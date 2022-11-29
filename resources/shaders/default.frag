#version 430 core

//#define WORLEY_MAX_CELLS_PER_AXIS 32
//#define WORLEY_MAX_NUM_POINTS WORLEY_MAX_CELLS_PER_AXIS*WORLEY_MAX_CELLS_PER_AXIS*WORLEY_MAX_CELLS_PER_AXIS
//#define WORLEY_FINE_OFFSET 0
//#define WORLEY_MEDIUM_OFFSET WORLEY_MAX_NUM_POINTS
//#define WORLEY_COARSE_OFFSET 2*WORLEY_MAX_NUM_POINTS

//const ivec3 CELL_OFFSETS[27] = {
//    ivec3(-1, -1, -1), ivec3(-1, -1, 0), ivec3(-1, -1, 1),
//    ivec3(-1,  0, -1), ivec3(-1,  0, 0), ivec3(-1,  0, 1),
//    ivec3(-1,  1, -1), ivec3(-1,  1, 0), ivec3(-1,  1, 1),
//    ivec3( 0, -1, -1), ivec3( 0, -1, 0), ivec3( 0, -1, 1),
//    ivec3( 0,  0, -1), ivec3( 0,  0, 0), ivec3( 0,  0, 1),
//    ivec3( 0,  1, -1), ivec3( 0,  1, 0), ivec3( 0,  1, 1),
//    ivec3( 1, -1, -1), ivec3( 1, -1, 0), ivec3( 1, -1, 1),
//    ivec3( 1,  0, -1), ivec3( 1,  0, 0), ivec3( 1,  0, 1),
//    ivec3( 1,  1, -1), ivec3( 1,  1, 0), ivec3( 1,  1, 1),
//};

//// Worley sample points and cell params,
//// updated when user changes cellsPerAxis
//layout(std430, binding = 0) buffer worleyBuffer {
//    // |       FINE      |       MEDIUM        |               COARSE               |
//    // 0......WORLEY_MAX_NUM_POINTS..2*WORLEY_MAX_NUM_POINTS..3*WORLEY_MAX_NUM_POINTS
//    vec4 worleyPoints[3*WORLEY_MAX_NUM_POINTS];
//};
//uniform int cellsPerAxisFine, cellsPerAxisMedium, cellsPerAxisCoarse;

//// sample wrapped worley density at position
//float sampleWorleyDensity(vec3 position, int offset, int cellsPerAxis) {
//    // [0, 1] in world-space maps to [0..cellsPerAxis) in volume space
//    const vec3 positionWrapped = fract(position);  // wrapped to [0, 1]^3
//    const ivec3 cellID = ivec3(positionWrapped * cellsPerAxis);  // [0..cellsPerAxis)^3
//    float minDist2 = 1.f;

//    // loop over all 27 adjacent cells and find out min distance
//    for (int offsetIndex = 0; offsetIndex < 27; offsetIndex++) {
//        const ivec3 adjID = cellID + CELL_OFFSETS[offsetIndex];  // [-1..cellsPerAxis]^3
//        const ivec3 adjIDWrapped = (adjID + cellsPerAxis) % cellsPerAxis;  // [0..cellsPerAxis)^3
//        const int adjCellIndex = adjIDWrapped.x + cellsPerAxis * (adjIDWrapped.y + cellsPerAxis * adjIDWrapped.z);
//        vec3 adjPosition = worleyPoints[offset + adjCellIndex].xyz;
//        // wrap point position in boundary cells
//        for (int comp = 0; comp < 3; comp++) {
//            if (adjID[comp] == -1) adjPosition[comp] -= 1.f;
//            else if (adjID[comp] == cellsPerAxis) adjPosition[comp] += 1.f;
//        }
//        const vec3 deltaPosition = positionWrapped - adjPosition;
//        minDist2 = min(minDist2, dot(deltaPosition, deltaPosition));
//    }

//    float density = sqrt(minDist2) * cellsPerAxis;

//    return density;
////    return max(0.f, density - densityThreshold);
//}


// density volumes computed by the compute shader
layout(binding = 0) uniform sampler3D volumeHighRes;
layout(binding = 1) uniform sampler3D volumeLowRes;

in vec3 positionWorld;
out vec4 glFragColor;

struct LightData {
    int type;
    vec4 pos;
    vec3 dir;
    vec3 color;
};

// volume transforms for computing ray-box intersection
uniform vec3 volumeScaling, volumeTranslate;

// ray origin, updated when user moves camera
uniform vec3 rayOrigWorld;

// rendering params, updated when user changes settings
uniform bool invertDensity;
uniform float densityMult;
//uniform float stepSize;
uniform int numSteps;


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
uniform int numLights;
uniform LightData lights[10];


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

float getErosionWeightCubic(float density) {
    return (1.f - density) * (1.f - density) * (1.f - density);
}

float sampleDensity(vec3 position) {
    // sample high-res details
    vec3 hiResPosition = position * hiResNoiseScaling * .1f + hiResNoiseTranslate * .1f;
    vec4 hiResNoise = texture(volumeHighRes, hiResPosition);
    float hiResDensity = dot( hiResNoise, normalizeL1(hiResChannelWeights) );
    if (invertDensity)
        hiResDensity = 1.f - hiResDensity;
    // TODO: add height gradient
    float hiResDensityWithOffset = hiResDensity + hiResDensityOffset;

    // return early if there is no cloud
    if (hiResDensityWithOffset <= 0.f)
        return 0.f;

    // add in low-res details
    vec3 loResPosition = position * loResNoiseScaling * .1f + loResNoiseTranslate * .1f;
    vec4 loResNoise = texture(volumeLowRes, loResPosition);
    float loResDensity = dot( loResNoise, normalizeL1(loResChannelWeights) );
    loResDensity = 1.f - loResDensity;  // invert the low-density by default

    // Erosion: subtract low-res detail from hi-res noise, weighted such that
    // the erosion is more pronounced near the boudary of the cloud (low hiResDensity)
    float erosionWeight = getErosionWeightCubic(hiResDensity);
    float density = hiResDensityWithOffset - erosionWeight*loResDensityWeight * loResDensity;
    return max(density * densityMult, 0.f);
}


void main() {
    const vec3 rayDirWorld = normalize(positionWorld - rayOrigWorld);
    vec2 tHit = intersectBox(rayOrigWorld, rayDirWorld);

    // keep the near intersection in front in case camera is inside volume
    tHit.x = max(0.f, tHit.x);

    // starting from the near intersection, march the ray forward and sample
    const float dt = (tHit.y - tHit.x) / numSteps;
    const vec3 ds = rayDirWorld * dt;
    vec3 pointWorld = rayOrigWorld + tHit.x * rayDirWorld;
    glFragColor = vec4(0.f);
    for (int step = 0; step < numSteps; step++) {
//        vec3 position = positionWorld * noiseScaling * .1f + noiseTranslate * .1f;
//        float sigma = sampleWorleyDensityFine(pointWorld) * densityMult;
//        sigma *= dt;
//        const vec3 rgb = vec3(0.f);
//        glFragColor.rgb += (1.f - glFragColor.a) * sigma * rgb;
//        glFragColor.a   += (1.f - glFragColor.a) * sigma;
//        if (glFragColor.a > 0.98)  // early stopping
//            break;
//        pointWorld += ds;
    }

//    glFragColor.r = linear2srgb(glFragColor.r);
//    glFragColor.g = linear2srgb(glFragColor.g);
//    glFragColor.b = linear2srgb(glFragColor.b);

    float sigma = sampleDensity(positionWorld);


    // DEBUG
//    vec3 position = positionWorld * hiResNoiseScaling * .1f + hiResNoiseTranslate * .1f;
    // show noise channels as BW images
//    float sigma = texture(volumeHighRes, position).r;
//    float sigma = texture(volumeHighRes, position).g;
//    float sigma = texture(volumeHighRes, position).b;
//    float sigma = texture(volumeHighRes, position).a;
//    float sigma = texture(volumeLowRes, position).r;
//    float sigma = texture(volumeLowRes, position).g;
//    float sigma = texture(volumeLowRes, position).b;
//    float sigma = texture(volumeLowRes, position).a;
    glFragColor = vec4(sigma);

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

