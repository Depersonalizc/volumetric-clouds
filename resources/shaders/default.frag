#version 430 core

#define EARLY_STOP_THRESHOLD 1e-2f
#define EARLY_STOP_LOG_THRESHOLD -4.6f
#define FOUR_PI 12.5663706144
#define XZ_FALLOFF_DIST 1.f
#define Y_FALLOFF_DIST 1.f

// Params for adaptive ray marching
#define MIN_NUM_FINE_STEPS 16
#define MAX_NUM_MISSED_STEPS 5
#define STEPSIZE_FINE 0.01f
#define COARSE_STEPSIZE_MULTIPLIER 10.f

// density volumes computed by the compute shader
layout(binding = 0) uniform sampler3D volumeHighRes;
layout(binding = 1) uniform sampler3D volumeLowRes;
layout(binding = 2) uniform sampler2D solidDepth;
layout(binding = 3) uniform sampler2D solidColor;

//in vec3 positionWorld;
in vec2 uv;
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
uniform float minLightTransmittance = 0.2f;

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

// Camera
uniform float xMax, yMax;  // rayDirWorldspace lies within [-xMax, xMax] x [-yMax, yMax] x {1.0}
uniform float near, far;   // terrain camera


// light uniforms, not used rn
struct LightData {
    int type;
    vec4 pos;
    vec3 dir;  // towards light source
    vec3 color;
    float longitude;
    float latitude;
} light;

uniform vec4 phaseParams;  // HG
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

float linearizeDepth(float depth) {
    float z = depth * 2.0 - 1.0; // Back to NDC, [0, 1] -> [-1, 1]
    return (2.0 * near * far) / (far + near - z * (far - near));  // Linearize z, [-1, 1] -> [near, far]
}

float depth2RayLength(float z) {
    vec2 _uv = 2.f * uv - 1.f;
    float x = _uv[0] * xMax;
    float y = _uv[1] * yMax;
    return sqrt(x*x + y*y + z*z);
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

float getErosionWeightQuntic(float density) {
    return pow( (1.f - density), 6 ) ;
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
    float ymax = ymin + volumeScaling.y;
    float distY = min(Y_FALLOFF_DIST, min(position.y - ymin, ymax - position.y));
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
    // Sample high-res shape textures
    const vec3 hiResPosition = position * hiResNoiseScaling * .1f + hiResNoiseTranslate;
    const vec4 hiResNoise = texture(volumeHighRes, hiResPosition);
    float hiResDensity = dot( hiResNoise, normalizeL1(hiResChannelWeights) );
    if (invertDensity)
        hiResDensity = 1.f - hiResDensity;

    // Reduce density at the bottom of the cloud to create crisp shape
    float falloff = yFalloff(position) * xzFalloff(position);
    hiResDensity *= falloff;

    // Control the cover of clouds by offsetting density
    float hiResDensityWithOffset = hiResDensity + hiResDensityOffset;

    // Skip adding details if there is no cloud to begin with
    if (hiResDensityWithOffset <= 0.f)
        return 0.f;

    // Sample low-res detail textures
    const vec3 loResPosition = position * loResNoiseScaling * .1f + loResNoiseTranslate;
    const vec4 loResNoise = texture(volumeLowRes, loResPosition);
    float loResDensity = dot( loResNoise, normalizeL1(loResChannelWeights) );
    loResDensity = 1.f - loResDensity;  // invert the low-res density by default

    // Detail erosion: subtract low-res detail from hi-res noise, weighted as such that
    // the erosion is more pronounced near the boudary of the cloud (low hiResDensity)
//    const float erosionWeight = getErosionWeightCubic(hiResDensity);
    const float erosionWeight = getErosionWeightQuntic(hiResDensity);

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

//    return lightTransmittance;
    return minLightTransmittance + lightTransmittance * (1.f - minLightTransmittance);
//    return 1.f;
}

void main() {

    // Solid geometry
    const float zSolid = linearizeDepth( texture(solidDepth, uv).r );
    const float tHitSolid = depth2RayLength(zSolid);
    const vec4 colorSolid = texture(solidColor, uv);

    const vec3 rayDirWorld = normalize(rayDirWorldspace);
    vec2 tHit = intersectBox(rayOrigWorld, rayDirWorld);
    tHit.x = max(tHit.x, 0.f);  // keep the near intersection in front of the camera
    tHit.y = min(tHit.y, tHitSolid);  // keep far intersection in front of solid geometry

    const vec3 dirLight = dirSph2Cart(radians(testLight.latitude), radians(testLight.longitude));  // towards the light
    const float cosRayLightAngle = dot(rayDirWorld, dirLight);
    const float phaseVal = phase(cosRayLightAngle);  // directional light only for now

    vec3 cloudColor = vec3(0.f);
    float transmittance = 1.f;
    float lightEnergy = 0.f;

    // Ray hit the box, let's do volume rendering!
    if (tHit.x < tHit.y) {
//        // Mode A. fixed number of steps
//        const float dt = (tHit.y - tHit.x) / numSteps;
//        const vec3 ds = rayDirWorld * dt;

        // Mode B. adaptive step size
        const float distTotal = (tHit.y - tHit.x);
        const float stepSizeFine = min(STEPSIZE_FINE, distTotal / MIN_NUM_FINE_STEPS);
        const float stepSizeCoarse = min(stepSizeFine * COARSE_STEPSIZE_MULTIPLIER, distTotal);
        const float distBackTrack = .5f * stepSizeCoarse;

        // Initialize all the variables
        int fineStepsLeft = MAX_NUM_MISSED_STEPS;
        float distTravelled = 0;
        float dt = stepSizeCoarse;  // start with coarse steps
        bool stepIsFine = false;
        vec3 pointWorld = rayOrigWorld + tHit.x * rayDirWorld;

        // Optionally apply random offset on ray start to minimize color banding
        const int seed = int(gl_FragCoord.y + 3000 * gl_FragCoord.x);
        const float eps = wangHash(seed);
        const float offset = eps * stepSizeCoarse;  // max offset is one coarse step
        pointWorld += offset * rayDirWorld;
        distTravelled += offset;

        // Raymarching starts
        while (distTravelled < distTotal) {

            distTravelled += dt;
            pointWorld += rayDirWorld * dt;

            const float density = sampleDensity(pointWorld);

            // Hit the cloud
            if (density > 0.f) {
                if (!stepIsFine) {
                    // Backtrack half a coarse step and switch to
                    // fine mode if the previous step was coarse
                    distTravelled -= distBackTrack;
                    pointWorld -= rayDirWorld * distBackTrack;
                    dt = stepSizeFine;
                    stepIsFine = true;
                } else {
                    // Regular volume rendering pass
                    // Cast secondary ray to sun, accumulate energy and transmittance
                    const float lightTransmittance = computeLightTransmittance(pointWorld, dirLight);
                    lightEnergy += density * transmittance * lightTransmittance * phaseVal * dt;
                    transmittance *= exp(-density * cloudLightAbsorptionMult * dt);

                    // Early stopping
                    if (transmittance < EARLY_STOP_THRESHOLD) break;
                }

                // Reset fine steps countdown since we hit the clodu
                fineStepsLeft = MAX_NUM_MISSED_STEPS;

            } else if (--fineStepsLeft <= 0) {
                // Missed cloud, decrement fine steps countdown by one
                // If we've missed the clouds for too many steps, switch to coarse mode
                dt = stepSizeCoarse;
                stepIsFine = false;
            }

        }  // Raymarching ends

        cloudColor = lightEnergy * testLight.color;
    }

    vec3 colorComposite;
    vec3 backgroundColor;
    if (texture(solidDepth, uv).r == 1) {
        // composite sky background into the scene
        // TODO: use texture to get sky color
        backgroundColor = vec3(.0f, .5f, .64f);

        // composite sun into the scene
        const float MAX_SUN_INTENSITY = 10.f;
        float sunIntensity = henyeyGreenstein(cosRayLightAngle, .9999);
        sunIntensity = min(sunIntensity, MAX_SUN_INTENSITY) * transmittance;

        colorComposite = min(cloudColor + transmittance*backgroundColor, 1.f);
        colorComposite = colorComposite*(1 - sunIntensity) + testLight.color*sunIntensity;

    } else {
        backgroundColor = colorSolid.rgb;
        colorComposite = min(cloudColor + transmittance*backgroundColor, 1.f);
    }

    if (gammaCorrect)
        colorComposite = gammaCorrection(colorComposite);
    glFragColor = vec4(colorComposite, 1.f);

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


