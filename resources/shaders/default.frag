#version 430 core

#define EARLY_STOP_THRESHOLD 1e-2f
#define EARLY_STOP_LOG_THRESHOLD -4.6f
#define EPSILON_INTERSECT 1e-3f
#define FOUR_PI 4*3.1415926535898
#define XZ_FALLOFF_DIST 1.f
#define Y_FALLOFF_DIST 1.f
#define MAX_RANDOM_OFFSET 1e-2f

// Params for adaptive ray marching
#define SMALL_DST_SAMPLE_NUM 32
#define COARSE_MULTIPLE 5.f
#define SMALL_DENSITY 0.005f
#define ADAP_THRESHOLD 5
#define STEPSIZE_FINE 0.05f

// density volumes computed by the compute shader
layout(binding = 0) uniform sampler3D volumeHighRes;
layout(binding = 1) uniform sampler3D volumeLowRes;

//in vec3 positionWorld;
in vec3 rayDirWorldspace;
out vec4 glFragColor;

// volume transforms for computing ray-box intersection
uniform vec3 volumeScaling, volumeTranslate;

// ray origin, updated when user moves camera
uniform vec3 rayOrigWorld;

// rendering params, updated when user changes settings
//uniform float stepSize;
uniform int numSteps;
uniform bool invertDensity, gammaCorrect;
uniform float densityMult;
uniform float cloudLightAbsorptionMult;
uniform float minLightTransmittance;

// Params for high resolution noise
uniform float hiResNoiseScaling;
uniform vec3 hiResNoiseTranslate;  // noise transforms
uniform vec4 hiResChannelWeights;  // how to aggregate RGBA channels
uniform float hiResDensityOffset;  // controls overall cloud coverage

// Params for low resolution noise
uniform float loResNoiseScaling;
uniform vec3 loResNoiseTranslate;  // noise transforms
uniform vec4 loResChannelWeights;  // how to aggregate RGBA channels
uniform float loResDensityWeight;  // relative weight of lo-res noise about hi-res



// light uniforms, not used rn
struct LightData {
    int type;
    vec4 pos;
    vec3 dir;  // towards light source
    vec3 color;
    float longitude;
    float latitude;
} light;

uniform LightData lightSource;
uniform vec4 phaseParams;  // HG
//const LightData testLight = LightData(0, vec4(0), vec3(0, -1, -0.3), vec3(1,0.1,0.5));
//const LightData testLight = LightData(0, vec4(0), vec3(0, 1, 0), vec3(1,1,1), 0, 0);
uniform LightData testLight;

vec3 shpere2Cartesian(vec3 curPnt) {
    float x, y, z;
    x = curPnt[0]*sin(curPnt[2])*sin(curPnt[1]);
    y = curPnt[0]*cos(curPnt[2]);
    z = curPnt[0]*sin(curPnt[2])*cos(curPnt[1]);
    return vec3(x,y,z);
}

// normalized v so that dot(v, 1) = 1
vec4 normalizeL1(vec4 v) {
    return v / dot(v, vec4(1.f));
}

// gamma correction
float linear2srgb(float x) {
    if (x <= 0.0031308f)
        return 12.92f * x;
    return 1.055f * pow(x, 1.f / 2.4f) - 0.055f;
}

vec3 gammaCorrection(vec3 linearRGB) {
    return vec3(linear2srgb(linearRGB.r), linear2srgb(linearRGB.g), linear2srgb(linearRGB.b));
}

// fast AABB intersection
vec2 intersectBox(vec3 orig, vec3 dir) {
    const vec3 boxMin = -.5f * volumeScaling + volumeTranslate;
    const vec3 boxMax = +.5f * volumeScaling + volumeTranslate;
    const vec3 invDir = 1.0 / dir;
    const vec3 tmin_tmp = (boxMin - orig) * invDir;
    const vec3 tmax_tmp = (boxMax - orig) * invDir;
    const vec3 tmin = min(tmin_tmp, tmax_tmp);
    const vec3 tmax = max(tmin_tmp, tmax_tmp);
    const float tn = max(tmin.x, max(tmin.y, tmin.z));
    const float tf = min(tmax.x, min(tmax.y, tmax.z));
    return vec2(tn, tf);
}

// Pseudo-random number generator that approximtes U(0, 1)
// http://www.reedbeta.com/blog/quick-and-easy-gpu-random-numbers-in-d3d11/
float wangHash(int seed) {
    seed = (seed ^ 61) ^ (seed >> 16);
    seed *= 9;
    seed = seed ^ (seed >> 4);
    seed *= 0x27d4eb2d;
    seed = seed ^ (seed >> 15);
    return float(seed % 2147483647) / 2147483647.f;
}

float getErosionWeightCubic(float density) {
    return (1.f - density) * (1.f - density) * (1.f - density);
}

// Henyey-Greenstein Phase Function
// inParam: float angle, float phaseParam (forwardScattering, backwardScattering) --> this is passed in hyperparam
// outParam: float phaseVal
float henyeyGreenstein(float cosTheta, float g) {
    const float g2 = g * g;
    return (1-g2) / (FOUR_PI * pow(1+g2-2*g*cosTheta, 1.5));
}

float phase(float cosTheta) {
    const float blend = 0.5;
    const float hgBlend = henyeyGreenstein(cosTheta, phaseParams.x) * (1-blend)
                        + henyeyGreenstein(cosTheta, phaseParams.y) * (blend);
    return phaseParams.z + hgBlend * phaseParams.w;
}

float yFalloff(vec3 position) {
    float ymin = -.5f * volumeScaling.y + volumeTranslate.y;
    float distY = min(Y_FALLOFF_DIST, position.y - ymin);
    return distY / Y_FALLOFF_DIST;
}

float xzFalloff(vec3 position) {
    float xmin = -.5f * volumeScaling.x + volumeTranslate.x;
    float xmax = xmin + volumeScaling.x;
    float zmin = -.5f * volumeScaling.z + volumeTranslate.z;
    float zmax = zmin + volumeScaling.z;
    float distX = min(XZ_FALLOFF_DIST, min(position.x - xmin, xmax - position.x));
    float distZ = min(XZ_FALLOFF_DIST, min(position.z - zmin, zmax - position.z));
    return min(distX, distZ) / XZ_FALLOFF_DIST;
}

float sampleDensity(vec3 position) {
    // sample high-res details
    const vec3 hiResPosition = position * hiResNoiseScaling * .1f + hiResNoiseTranslate;
    const vec4 hiResNoise = texture(volumeHighRes, hiResPosition);
    float hiResDensity = dot( hiResNoise, normalizeL1(hiResChannelWeights) );
    if (invertDensity)
        hiResDensity = 1.f - hiResDensity;

    // reduce density at the bottom of the cloud to create crisp shape
    float falloff = yFalloff(position);
    hiResDensity *= falloff;

    float hiResDensityWithOffset = hiResDensity + hiResDensityOffset;

    // return early if there is no cloud
    if (hiResDensityWithOffset <= 0.f)
        return 0.f;

    // add in low-res details
    const vec3 loResPosition = position * hiResNoiseScaling * .1f + loResNoiseTranslate;
    const vec4 loResNoise = texture(volumeLowRes, loResPosition);
    float loResDensity = dot( loResNoise, normalizeL1(loResChannelWeights) );
    loResDensity = 1.f - loResDensity;  // invert the low-res density by default

    // detail erosion: subtract low-res detail from hi-res noise, weighted such that
    // the erosion is more pronounced near the boudary of the cloud (low hiResDensity)
    const float erosionWeight = getErosionWeightCubic(hiResDensity);
    const float density = hiResDensityWithOffset - erosionWeight*loResDensityWeight * loResDensity;
    return max(density * densityMult*10.f, 0.f);
}

// One-bounce raymarch to get light transmittance
float computeLightTransmittance(vec3 rayOrig, vec3 rayDir) {
    const int numStepsRecursive = numSteps / 8;
    const vec2 tHit = intersectBox(rayOrig, rayDir);
    const float tFar = max(0.f, tHit.y);
    const float dt = tFar / numStepsRecursive;
    const vec3 ds = rayDir * dt;

    float tau = 0.f;  // log transmittance
    vec3 pointWorld = rayOrig;
    for (float t = 0.f; t < tFar; t += dt) {
        const float density = sampleDensity(pointWorld);
        tau -= density;
        pointWorld += ds;
    }
    tau *= (cloudLightAbsorptionMult * dt);  // delay multiplication to save compute and avoid precision issues
    float lightTransmittance = exp(tau);

    return lightTransmittance;
//    return minLightTransmittance + lightTransmittance * (1.f - minLightTransmittance);
//    return 1.f;
}

//------------Skycolor-------------------
// Simulates an atmosphere
// return the distance traveled inside the atmosphere
float raySphere(vec3 sphereCenter, float sphereRadius, vec3 rayOrigin, vec3 rayDir) {
    vec3 offset = rayOrigin - sphereCenter;
    float a = 1;
    float b = 2 * dot(offset, rayDir);
    float c = dot(offset, offset) - sphereRadius * sphereRadius;
    float d = b*b - 4*a*c;

    if (d > 0) {
        float s = sqrt(d);
        float dstNear = max(0, (-b-s)/(2*a));
        float dstFar = (-b+s)/(2*a);
        if (dstFar >= 0) {
            return dstFar - dstNear;
        }
    }
    return 0.0;
}

// Used for sky scattering
float densityAtPoint(vec3 pointPosWorld) {
    float densityFalloff = 8.0;
    float height01 = pointPosWorld[1] / 2.0;
    float density = exp(- height01 * densityFalloff) * (1 - height01);
    return density;
}

// optical depth: average density along the ray, determined by the raylength (from point to the sun, within the box)
float opticalDepth(vec3 rayOrig, vec3 rayDir, float rayLength) {
//    const vec2 tHit = intersectBox(rayOrig, rayDir);
//    const float tFar = max(0.f, tHit.y);
//    return tFar / 10.0;
    int numOpticalPoints = 6;
    vec3 densitySamplePoint = rayOrig;
    float stepSize = rayLength / (numOpticalPoints - 1);
    float opticalDepth = 0;
    for (int i = 0; i < numOpticalPoints; i++) {
        float localDensity = densityAtPoint(densitySamplePoint);
        opticalDepth += localDensity * stepSize;
        densitySamplePoint += rayDir * stepSize;
    }
    return opticalDepth;
}


void main() {

//    const vec3 rayDirWorld = normalize(positionWorld - rayOrigWorld);
    const vec3 rayDirWorld = normalize(rayDirWorldspace);
    vec2 tHit = intersectBox(rayOrigWorld, rayDirWorld);
//    tHit.x = max(0.f, tHit.x);
    tHit.x = max(0.f, tHit.x) + EPSILON_INTERSECT;  // keep the near intersection in front of the camera
    const vec3 dirLight = normalize(shpere2Cartesian(vec3(1, radians(testLight.latitude), radians(testLight.longitude))));  // towards the light
//    const vec3 dirLight = normalize(testLight.dir);  // towards the light
    const float cosRayLightAngle = dot(rayDirWorld, dirLight);
    const float phaseVal = phase(cosRayLightAngle);  // directional light only for now

    vec3 cloudColor = vec3(0.f);
    float transmittance = 1.f;
    float lightEnergy = 0.f;

    //----------------------skycolor related-------------------
    vec3 inScatteredLight = vec3(0.0, 0.0, 0.0);
    float scatteringStrength = 1;
    vec3 wavelengths = vec3(700, 530, 440);
    float scatterR = pow(400 / wavelengths[0], 4) * scatteringStrength;
    float scatterG = pow(400 / wavelengths[1], 4) * scatteringStrength;
    float scatterB = pow(400 / wavelengths[2], 4) * scatteringStrength;
    vec3 scatteringCoeff = vec3(scatterR, scatterG, scatterB);

    float viewRayOpticalDepth = 0.0;
    int numInScatteringPoints = 8;

    // Create atmosphere
//    vec3 planetCenter = vec3(0.0, - 11.0 * volumeScaling.y, 0.0);
//    float atmosRadius = 13.0 * volumeScaling.y;
    vec3 planetCenter = vec3(0.0, - 11.0, 0.0);
    float atmosRadius = 13.0 ;
    float planetRadius = 11.0;

//    vec3 planetCenter = vec3(0.0, - 111.0, 0.0);
//    float atmosRadius = 113.0 ;

//    float planetRadius = 1000.f;
//    float atmosRadius = 1003.0;
//    vec3 planetCenter = vec3(0.0, -planetRadius-5, 0.0);

    vec3 color = testLight.color;


    if (tHit.x < tHit.y) {  // hit box
        // starting from the near intersection, march the ray forward and sample
        float dstTravelled = 0;
        float totalDst = (tHit.y - tHit.x);
        float curFineStepSize = min(STEPSIZE_FINE, totalDst/SMALL_DST_SAMPLE_NUM);
        float curCoarseStepSize = curFineStepSize*COARSE_MULTIPLE;
        int curThreshold = ADAP_THRESHOLD;
//        const float dt = (tHit.y - tHit.x) / numSteps;
        float dt = curFineStepSize;
        const vec3 ds = rayDirWorld * dt;

        vec3 pointWorld = rayOrigWorld + tHit.x * rayDirWorld;

        while (dstTravelled < totalDst){
            float density = sampleDensity(pointWorld);

            dstTravelled += dt;

//            //----------------------------skycolor related-------------------------------
//            float localDensity = densityAtPoint(pointWorld);
//            float sunRayLength = raySphere(planetCenter, atmosRadius, pointWorld, dirLight);

//            float sunRayOpticalDepth = opticalDepth(pointWorld, dirLight, sunRayLength);
////            sunRayOpticalDepth = sunRayLength / 10.0;
////            viewRayOpticalDepth = dstTravelled / 10.0;
//            viewRayOpticalDepth = opticalDepth(pointWorld, -rayDirWorld, dstTravelled);
////            vec3 transSky = exp(- (sunRayOpticalDepth + viewRayOpticalDepth )* scatteringCoeff);
//            vec3 transSky = exp(- (sunRayOpticalDepth + viewRayOpticalDepth)* scatteringCoeff);
//            inScatteredLight += localDensity * transSky * scatteringCoeff * dt;


            if (density > 0.f) {
                float lightTransmittance = computeLightTransmittance(pointWorld, dirLight);
                lightEnergy += density * transmittance * lightTransmittance * phaseVal * dt;
                transmittance *= exp(-density * cloudLightAbsorptionMult * dt);
                if (transmittance < EARLY_STOP_THRESHOLD)
                    break;

                // adjust next step
                curThreshold = density < SMALL_DENSITY ? curThreshold - 1: ADAP_THRESHOLD;
                // change to big step
                if (curThreshold <= 0) {
                    dt = curCoarseStepSize;
                // check change to small step
                } else {
                    // if prev is big step, traceback and change to small step
                    if (dt == curCoarseStepSize) {
                        dt = curFineStepSize;
                    }
                }
            }
            pointWorld += dt*rayDirWorld;
        }

//        color  += inScatteredLight;
        cloudColor = lightEnergy * color;
    }

    //----------------------------skycolor related-------------------------------
    vec3 pointWorld = rayOrigWorld;
    float rayLength = raySphere(planetCenter, atmosRadius, pointWorld, normalize(rayDirWorld));
    float stepSize = rayLength / (numInScatteringPoints - 1);

    for (int i = 0; i < numInScatteringPoints; i++) {

        float localDensity = densityAtPoint(pointWorld);
        float sunRayLength = raySphere(planetCenter, atmosRadius, pointWorld, dirLight);

        float sunRayOpticalDepth = opticalDepth(pointWorld, dirLight, sunRayLength);
//            sunRayOpticalDepth = sunRayLength / 10.0;
//            viewRayOpticalDepth = dstTravelled / 10.0;
        viewRayOpticalDepth = opticalDepth(pointWorld, -rayDirWorld, stepSize * i);
        vec3 transSky = exp(- (sunRayOpticalDepth + viewRayOpticalDepth)* scatteringCoeff);
        inScatteredLight += localDensity * transSky * scatteringCoeff * stepSize;
        pointWorld += rayDirWorld * stepSize;
    }


    vec3 backgroundColor = vec3(.0f, .0f, .0f);

    if (raySphere(planetCenter, planetRadius, rayOrigWorld, rayDirWorld) <= 0.0) {
        vec3 originalColor = vec3(0.0, 0.0, 0.0);
        float originalColTrans = exp(- viewRayOpticalDepth);
        backgroundColor = originalColor * originalColTrans + inScatteredLight;
    }
    // composite sky background into the scene
    // TODO: use texture to get sky color
//    vec3 backgroundColor = vec3(.0f, .5f, .64f);

//    backgroundColor += inScatteredLight;



    // composite sun into the scene
    const float MAX_SUN_INTENSITY = 10.f;
    float sunIntensity = henyeyGreenstein(cosRayLightAngle, .9999);
    sunIntensity = min(sunIntensity, MAX_SUN_INTENSITY) * transmittance;

    vec3 cloudSky = min(cloudColor + transmittance*backgroundColor, 1.f);
    cloudSky = cloudSky*(1 - sunIntensity) + testLight.color*sunIntensity;

    if (gammaCorrect)
        cloudSky = gammaCorrection(cloudSky);
    glFragColor = vec4(cloudSky, 1.f);
//    glFragColor = vec4(testLight.color, 1.f);

    // DEBUG
//    float sigma = sampleDensity(positionWorld);
//    vec3 position = positionWorld * hiResNoiseScaling * .1f + hiResNoiseTranslate * .1f;
    // show noise channels as BW images
//    float sigma = sampleDensity(positionWorld);
//    float sigma = texture(volumeHighRes, position).g;
//    float sigma = texture(volumeHighRes, position).b;
//    float sigma = texture(volumeHighRes, position).a;
//    float sigma = texture(volumeLowRes, position).r;
//    float sigma = texture(volumeLowRes, position).g;
//    float sigma = texture(volumeLowRes, position).b;
//    float sigma = texture(volumeLowRes, position).a;
//    glFragColor = vec4(sigma);

    // show noise channels as is
//    glFragColor = vec4(1.f);
//    glFragColor.r = texture(volumeHighRes, position).r;
//    glFragColor.g = texture(volumeHighRes, position).g;
//    glFragColor.b = texture(volumeHighRes, position).b;
//    glFragColor.r = texture(volumeHighRes, position).a;

    // final processing
//    if (invertDensity)
//        glFragColor = 1.f - glFragColor;
//    glFragColor *= densityMult;
//    glFragColor.a = 1.f;

}

