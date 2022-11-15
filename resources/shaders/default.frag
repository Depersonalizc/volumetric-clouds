#version 330 core

struct LightData {
    int type;
    vec4 pos;
    vec3 dir;
    vec3 color;
};

in vec3 positionCube;  // frag position in [0, 1]^3 cube space

out vec4 glFragColor;

// rendering params
uniform float intervalLength;

// ray uniform
uniform vec3 rayOrigCube;

// volume uniform
uniform vec3 volumeDims;    // world space dimensions with longest axis having length 1
uniform float volumeScale;  // the unifom scale factor of volume from voxel space to world space

// light uniforms
uniform int numLights;
uniform LightData lights[10];

uniform sampler3D volume;


// fast AABB intersection
vec2 intersectBox(vec3 orig, vec3 dir) {
    const vec3 boxMin = vec3(0);
    const vec3 boxMax = vec3(1);
    vec3 invDir = 1.0 / dir;
    vec3 tmin_tmp = (boxMin - orig) * invDir;
    vec3 tmax_tmp = (boxMax - orig) * invDir;
    vec3 tmin = min(tmin_tmp, tmax_tmp);
    vec3 tmax = max(tmin_tmp, tmax_tmp);
    float tn = max(tmin.x, max(tmin.y, tmin.z));
    float tf = min(tmax.x, min(tmax.y, tmax.z));
    return vec2(tn, tf);
}

float maxComp(vec3 v) {
    return max(v.x, max(v.y, v.z));
}

float linear2srgb(float x) {
    if (x <= 0.0031308f)
        return 12.92f * x;
    return 1.055f * pow(x, 1.f / 2.4f) - 0.055f;
}

void main() {
    vec3 rayDirCube  = positionCube - rayOrigCube;  // NOTE: unnormalized
    vec3 rayDirWorld = rayDirCube * volumeDims;     // NOTE: unnormalized

    vec2 tHit = intersectBox(rayOrigCube, rayDirCube);

    // keep the near intersection in front in case camera is inside volume
    tHit.x = max(0.f, tHit.x);

    // compute step size dt such that each voxel is sampled at least once
    // i.e., dt * (rayDirCube * volumeDims * longestAxis) has length = 1
    float dt = 1.f / length(rayDirWorld) * volumeScale;

    // starting from the near intersection, march the ray forward and sample
    glFragColor = vec4(0.f);
    vec3 ds = rayDirCube * dt;
    vec3 pointCube = rayOrigCube + tHit.x * rayDirCube;
    for (float t = tHit.x; t < tHit.y; t += dt) {
        float sigma = texture(volume, pointCube).r * volumeScale * intervalLength;
        const vec3 rgb = vec3(1.f);

        glFragColor.rgb += (1.f - glFragColor.a) * sigma * rgb;
        glFragColor.a   += (1.f - glFragColor.a) * sigma;

        if (glFragColor.a > 0.95)  // early stopping
            break;

        pointCube += ds;
    }

    glFragColor.r = linear2srgb(glFragColor.r);
    glFragColor.g = linear2srgb(glFragColor.g);
    glFragColor.b = linear2srgb(glFragColor.b);
//    glFragColor.a = 1.f - glFragColor.a;

//    glFragColor = vec4(  max(normalize(rayDirCube), 0.f), 1.f);
//    glFragColor = vec4(  max(rayDirWorld, 0.f), 1.f);
//    glFragColor = vec4(vec3(exp(-tHit.x)), 1.f);
//    glFragColor = vec4(vec3(dt), 1.f);
//    glFragColor = vec4(1.f, 1.f, 1.f, 0.5f);
//    glFragColor = vec4(texture(volume, vec3(.7f)).r);
}
