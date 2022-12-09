#version 430 core

#define EARLY_STOP_THRESHOLD 1e-2f
#define EARLY_STOP_LOG_THRESHOLD -4.6f
#define EPSILON_INTERSECT 1e-3f
#define FOUR_PI 4*3.1415926535898
#define XZ_FALLOFF_DIST 1.f
#define Y_FALLOFF_DIST 1.f
#define MAX_RANDOM_OFFSET 1e-2f

// Params for adaptive ray marching
#define SMALL_DST_SAMPLE_NUM 64
#define COARSE_STEPSIZE_MULTIPLIER 10.f
//#define SMALL_DENSITY 0.005f
#define MAX_MISSED_STEPS 5
#define STEPSIZE_FINE 0.01f

// density volumes computed by the compute shader
layout(binding = 0) uniform sampler3D volumeHighRes;
layout(binding = 1) uniform sampler3D volumeLowRes;
layout(binding = 2) uniform sampler2D solidDepth;
layout(binding = 3) uniform sampler2D solidColor;

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

vec3 dirSph2Cart(float latitudeRadians, float longitudeRadians) {
    float x, y, z;
    x = sin(longitudeRadians) * sin(latitudeRadians);
    y = cos(longitudeRadians);
    z = sin(longitudeRadians) * cos(latitudeRadians);
    return vec3(x, y, z);
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

void main() {

    const vec3 rayDirWorld = normalize(rayDirWorldspace);
    vec2 tHit = intersectBox(rayOrigWorld, rayDirWorld);

//    tHit.x = max(0.f, tHit.x);
    tHit.x = max(0.f, tHit.x) + EPSILON_INTERSECT;  // keep the near intersection in front of the camera

    const vec3 dirLight = dirSph2Cart(radians(testLight.latitude),
                                      radians(testLight.longitude));  // towards the light
//    const vec3 dirLight = normalize(testLight.dir);  // towards the light
    const float cosRayLightAngle = dot(rayDirWorld, dirLight);
    const float phaseVal = phase(cosRayLightAngle);  // directional light only for now

    vec3 cloudColor = vec3(0.f);
    float transmittance = 1.f;
    float lightEnergy = 0.f;
    if (tHit.x < tHit.y) {  // hit box, do volume render

        // Mode A. adaptive step size
        const float distTotal = (tHit.y - tHit.x);
        const float stepSizeFine = min(STEPSIZE_FINE, distTotal/SMALL_DST_SAMPLE_NUM);
        const float stepSizeCoarse = stepSizeFine * COARSE_STEPSIZE_MULTIPLIER;

        int fineStepsLeft = MAX_MISSED_STEPS;
        float distTravelled = 0;
        float dt = stepSizeFine;
        bool stepIsFine = true;

//        // Mode B. fixed number of steps
//        const float dt = (tHit.y - tHit.x) / numSteps;
//        const vec3 ds = rayDirWorld * dt;

        vec3 pointWorld = rayOrigWorld + tHit.x * rayDirWorld;
        while (distTravelled < distTotal) {

            distTravelled += dt;
            pointWorld += rayDirWorld * dt;

            const float density = sampleDensity(pointWorld);

            if (density > 0.f) {

                // hit cloud, backtrack half a coarse step if previous step was coarse
                if (!stepIsFine) {
                    distTravelled -= (stepSizeCoarse * .5f);
                    pointWorld -= (rayDirWorld * stepSizeCoarse * .5f);
                    // switch to fine mode
                    dt = stepSizeFine;
                    stepIsFine = true;
                }

                else {
                    // accumulate transmittance and energy
                    const float lightTransmittance = computeLightTransmittance(pointWorld, dirLight);
                    lightEnergy += density * transmittance * lightTransmittance * phaseVal * dt;
                    transmittance *= exp(-density * cloudLightAbsorptionMult * dt);

                    // cast secondary ray to sun
                    if (transmittance < EARLY_STOP_THRESHOLD) break;

                    // hit cloud, reset fine steps countdown
                    fineStepsLeft = MAX_MISSED_STEPS;
                }

                // hit cloud, reset fine steps count down
                fineStepsLeft = MAX_MISSED_STEPS;

            } else {
                // missed cloud, decrement fine steps countdown
                fineStepsLeft = fineStepsLeft - 1;

                // if we've missed the clouds for too many steps, switch to coarse mode
                if (fineStepsLeft <= 0) {
                    dt = stepSizeCoarse;
                    stepIsFine = false;
                }
            }

        }

        cloudColor = lightEnergy * testLight.color;
    }

    // composite sky background into the scene
    // TODO: use texture to get sky color
    vec3 backgroundColor = vec3(.0f, .5f, .64f);

    // composite sun into the scene
    const float MAX_SUN_INTENSITY = 10.f;
    float sunIntensity = henyeyGreenstein(cosRayLightAngle, .9999);
    sunIntensity = min(sunIntensity, MAX_SUN_INTENSITY) * transmittance;

    vec3 cloudSky = min(cloudColor + transmittance*backgroundColor, 1.f);
    cloudSky = cloudSky*(1 - sunIntensity) + testLight.color*sunIntensity;

    if (gammaCorrect)
        cloudSky = gammaCorrection(cloudSky);
    glFragColor = vec4(cloudSky, 1.f);


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

