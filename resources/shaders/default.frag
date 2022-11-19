#version 430 core

const ivec3 CELL_OFFSETS[27] = {
    ivec3(-1, -1, -1), ivec3(-1, -1, 0), ivec3(-1, -1, 1),
    ivec3(-1,  0, -1), ivec3(-1,  0, 0), ivec3(-1,  0, 1),
    ivec3(-1,  1, -1), ivec3(-1,  1, 0), ivec3(-1,  1, 1),
    ivec3( 0, -1, -1), ivec3( 0, -1, 0), ivec3( 0, -1, 1),
    ivec3( 0,  0, -1), ivec3( 0,  0, 0), ivec3( 0,  0, 1),
    ivec3( 0,  1, -1), ivec3( 0,  1, 0), ivec3( 0,  1, 1),
    ivec3( 1, -1, -1), ivec3( 1, -1, 0), ivec3( 1, -1, 1),
    ivec3( 1,  0, -1), ivec3( 1,  0, 0), ivec3( 1,  0, 1),
    ivec3( 1,  1, -1), ivec3( 1,  1, 0), ivec3( 1,  1, 1),
};

struct LightData {
    int type;
    vec4 pos;
    vec3 dir;
    vec3 color;
};

in vec3 positionWorld;  // frag position in world space
out vec4 glFragColor;


// Worley sample points and cell params,
// updated when user changes cellsPerAxis
layout(std430, binding = 0) buffer worleyBufferFine {
    vec4 worleyPointsFine[];  // stratified sample points for generat Worley noise
};
layout(std430, binding = 1) buffer worleyBufferCoarse {
    vec4 worleyPointsCoarse[];  // stratified sample points for generat Worley noise
};
uniform int cellsPerAxisFine, cellsPerAxisCoarse;
//uniform int numPointsFine, numPointsCoarse;

// rendering params, updated when user changes settings
uniform bool invertDensity;
uniform float densityMult;
uniform float stepSize;

// Worley noise transforms
uniform float noiseScaling;
uniform vec3 noiseTranslate;

// volume transforms for computing ray intersection
uniform vec3 volumeScaling;
uniform vec3 volumeTranslate;

// ray origin, updated when user moves camera
uniform vec3 rayOrigWorld;

// light uniforms, not used rn
uniform int numLights;
uniform LightData lights[10];
// for test, not used rn
uniform sampler3D volume;


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

// sample wrapped worley density at position
float sampleWorleyDensityFine(vec3 position) {

    position = position * noiseScaling * 0.01f + noiseTranslate * 0.01f;

    // [0, 1] in world-space maps to [0..cellsPerAxis) in volume space
    const vec3 positionWrapped = fract(position);  // wrapped to [0, 1]^3
    const ivec3 cellID = ivec3(positionWrapped * cellsPerAxisFine);  // [0..cellsPerAxis)^3
    float minDist2 = 1.f;

    // loop over all 27 adjacent cells and find out min distance
    for (int offsetIndex = 0; offsetIndex < 27; offsetIndex++) {
        const ivec3 adjID = cellID + CELL_OFFSETS[offsetIndex];  // [-1..cellsPerAxis]^3
        const ivec3 adjIDWrapped = (adjID + cellsPerAxisFine) % cellsPerAxisFine;  // [0..cellsPerAxis)^3
        const int adjCellIndex = adjIDWrapped.x + cellsPerAxisFine * (adjIDWrapped.y + cellsPerAxisFine * adjIDWrapped.z);
        vec3 adjPosition = worleyPointsFine[adjCellIndex].xyz;
        // wrap point position in boundary cells
        for (int comp = 0; comp < 3; comp++) {
            if (adjID[comp] == -1) adjPosition[comp] -= 1.f;
            else if (adjID[comp] == cellsPerAxisFine) adjPosition[comp] += 1.f;
        }
        const vec3 deltaPosition = positionWrapped - adjPosition;
        minDist2 = min(minDist2, dot(deltaPosition, deltaPosition));
    }

    const float density = sqrt(minDist2) * cellsPerAxisFine;
    return density;
}

// sample wrapped worley density at position
float sampleWorleyDensityCoarse(vec3 position) {

    position = position * noiseScaling * 0.01f + noiseTranslate * 0.01f;

    // [0, 1] in world-space maps to [0..cellsPerAxis) in volume space
    const vec3 positionWrapped = fract(position);  // wrapped to [0, 1]^3
    const ivec3 cellID = ivec3(positionWrapped * cellsPerAxisCoarse);  // [0..cellsPerAxis)^3
    float minDist2 = 1.f;

    // loop over all 27 adjacent cells and find out min distance
    for (int offsetIndex = 0; offsetIndex < 27; offsetIndex++) {
        const ivec3 adjID = cellID + CELL_OFFSETS[offsetIndex];  // [-1..cellsPerAxis]^3
        const ivec3 adjIDWrapped = (adjID + cellsPerAxisCoarse) % cellsPerAxisCoarse;  // [0..cellsPerAxis)^3
        const int adjCellIndex = adjIDWrapped.x + cellsPerAxisCoarse * (adjIDWrapped.y + cellsPerAxisCoarse * adjIDWrapped.z);
        vec3 adjPosition = worleyPointsCoarse[adjCellIndex].xyz;
        // wrap point position in boundary cells
        for (int comp = 0; comp < 3; comp++) {
            if (adjID[comp] == -1) adjPosition[comp] -= 1.f;
            else if (adjID[comp] == cellsPerAxisCoarse) adjPosition[comp] += 1.f;
        }
        const vec3 deltaPosition = positionWrapped - adjPosition;
        minDist2 = min(minDist2, dot(deltaPosition, deltaPosition));
    }

    const float density = sqrt(minDist2) * cellsPerAxisCoarse;
    return density;
}



void main() {
    const vec3 rayDirWorld = normalize(positionWorld - rayOrigWorld);
    vec2 tHit = intersectBox(rayOrigWorld, rayDirWorld);

    // keep the near intersection in front in case camera is inside volume
    tHit.x = max(0.f, tHit.x);

    // starting from the near intersection, march the ray forward and sample
    const vec3 ds = rayDirWorld * stepSize;
    vec3 pointWorld = rayOrigWorld + tHit.x * rayDirWorld;
    glFragColor = vec4(0.f);
//    for (float t = tHit.x; t < tHit.y; t += stepSize) {
//        // TODO: Generate Worley
//        float sigma = sampleWorleyDensityFine(pointWorld) * densityMult;
//        sigma *= stepSize;

//        const vec3 rgb = vec3(1.f);

//        glFragColor.rgb += (1.f - glFragColor.a) * sigma * rgb;
//        glFragColor.a   += (1.f - glFragColor.a) * sigma;

//        if (glFragColor.a > 0.95)  // early stopping
//            break;

//        pointWorld += ds;
//    }

//    glFragColor.r = linear2srgb(glFragColor.r);
//    glFragColor.g = linear2srgb(glFragColor.g);
//    glFragColor.b = linear2srgb(glFragColor.b);

//    float sigma = sampleWorleyDensityFine(positionWorld);
//    float sigma = sampleWorleyDensityCoarse(positionWorld);
//    float sigma = (sampleWorleyDensityFine(positionWorld) +
//                   sampleWorleyDensityCoarse(positionWorld) ) * .5f;
    float sigma = (sampleWorleyDensityFine(positionWorld) *
                   sampleWorleyDensityCoarse(positionWorld) );


    if (invertDensity)
        sigma = 1.f - sigma;
    glFragColor.rgb = vec3(sigma * densityMult);
    glFragColor.a = 1.f;


// DEBUG
//    glFragColor = vec4(1.f);
//    glFragColor = worleyPointsFine[256*12*256-1];
//    glFragColor.a = 1.f - glFragColor.a;
//    glFragColor = vec4(  max(normalize(rayDirCube), 0.f), 1.f);
//    glFragColor = vec4(  max(rayDirWorld, 0.f), 1.f);
//    glFragColor = vec4(vec3(exp(-tHit.x)), 1.f);
//    glFragColor = vec4(vec3(dt), 1.f);
//    glFragColor = vec4(1.f, 1.f, 1.f, 0.5f);
//    glFragColor = vec4(texture(volume, vec3(.7f)).r);
}
