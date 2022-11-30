#version 430 core

#define EARLY_STOP_THRESHOLD 0.01f
#define EARLY_STOP_LOG_THRESHOLD -4.6f
#define EPSILON_INTERSECT 0.001f
#define FOUR_PI 4*3.1415926535898
#define XZ_FALLOFF_DIST 1.f
#define Y_FALLOFF_DIST 1.f

// density volumes computed by the compute shader
layout(binding = 0) uniform sampler3D volumeHighRes;
layout(binding = 1) uniform sampler3D volumeLowRes;

in vec3 positionWorld;
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
uniform float cloudLightAbsorptionMult = .75f;
uniform float minLightTransmittance = 0.01f;


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
};
uniform LightData lightSource;
uniform vec4 phaseParams;  // HG
//const LightData testLight = LightData(0, vec4(0), vec3(0, -1, -0.3), vec3(1,0.1,0.5));
const LightData testLight = LightData(0, vec4(0), vec3(0, -1, -0.3), vec3(1,1,1));


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
    float yt = (position.y - ymin) / volumeScaling.y;
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

    // reduce density at the bounday of the volume and at the bottom of the cloud
    float falloff = yFalloff(position) * xzFalloff(position);
    hiResDensity *= falloff;

    const float hiResDensityWithOffset = hiResDensity + hiResDensityOffset;

    // return early if there is no cloud
    if (hiResDensityWithOffset <= 0.f)
        return 0.f;

    // add in low-res details
    const vec3 loResPosition = position * loResNoiseScaling * .1f + loResNoiseTranslate;
    const vec4 loResNoise = texture(volumeLowRes, loResPosition);
    float loResDensity = dot( loResNoise, normalizeL1(loResChannelWeights) );
    loResDensity = 1.f - loResDensity;  // invert the low-density by default

    // detail erosion: subtract low-res detail from hi-res noise, weighted such that
    // the erosion is more pronounced near the boudary of the cloud (low hiResDensity)
    const float erosionWeight = getErosionWeightCubic(hiResDensity);
    const float density = hiResDensityWithOffset - erosionWeight*loResDensityWeight * loResDensity;
    return max(density * densityMult * 10.f, 0.f);
}

// One-bounce ray marching to get light attenuation
float rayMarch(vec3 rayOrig, vec3 rayDir) {
    const int numStepsRecursive = numSteps / 8;
    const vec2 tHit = intersectBox(rayOrig, rayDir);
    const float tFar = max(0.f, tHit.y);
    const float dt = tFar / numStepsRecursive;
    const vec3 ds = rayDir * dt;

    vec3 pointWorld = rayOrig;
    float tau = 0.f;  // log(transmittance)
    for (float t = 0.f; t < tFar; t += dt) {
        const float density = sampleDensity(pointWorld);
        tau -= density * cloudLightAbsorptionMult * dt;
        if (tau < EARLY_STOP_LOG_THRESHOLD)
            break;
        pointWorld += ds;
    }
    float lightTransmittance = exp(tau);

    return lightTransmittance;
//    return minLightTransmittance + lightTransmittance * (1.f - minLightTransmittance);
//    return 1.f;
}


void main() {
    const vec3 rayDirWorld = normalize(positionWorld - rayOrigWorld);
    vec2 tHit = intersectBox(rayOrigWorld, rayDirWorld);

    // keep the near intersection in front in case camera is inside volume
    tHit.x = max(0.f, tHit.x) + EPSILON_INTERSECT;

    // starting from the near intersection, march the ray forward and sample
    const float dt = (tHit.y - tHit.x) / numSteps;
    const vec3 ds = rayDirWorld * dt;

    float randomOffset = wangHash(int(gl_FragCoord.x + 5000 * gl_FragCoord.y));
    vec3 pointWorld = rayOrigWorld + tHit.x * rayDirWorld
                                   + (randomOffset * 1e-3f) * rayDirWorld;


    float lightEnergy = 0.f;
    float transmittance = 1.f;
    vec3 rayDirLight = normalize(testLight.dir);  // towards the light
    float cosAngle = dot(rayDirWorld, rayDirLight);
    float phaseVal = phase(cosAngle);  // directional light only for now

    glFragColor = vec4(0.f);
    for (int step = 0; step < numSteps; step++) {
        float density = sampleDensity(pointWorld);

        float lightTransmittance = rayMarch(pointWorld, rayDirLight);
//        float lightTransmittance = 1.f;

        lightEnergy += density * transmittance * lightTransmittance * phaseVal * dt;
        transmittance *= exp(-density * cloudLightAbsorptionMult * dt);

        if (transmittance < EARLY_STOP_THRESHOLD)
            break;

        pointWorld += ds;
    }

    vec3 cloudColor = lightEnergy * testLight.color;
    vec3 backgroundColor = vec3(.1f, .1f, .3f); // TODO: currently no background color contribute
    vec3 finalColor = cloudColor + transmittance*backgroundColor;

    if (gammaCorrect)
        finalColor = gammaCorrection(finalColor);
    glFragColor = vec4(finalColor, 1.f);


//    float sigma = sampleDensity(positionWorld);


    // DEBUG
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
    glFragColor.a = 1.f;

}

