#version 330 core

layout(location = 0) in vec3 position;  // cube [0, 1]^3

out vec3 positionCube;  // ray origin and ray direction in [0, 1]^3 cube space


uniform mat4 projView;
//uniform vec3 camPosWorld;

uniform vec3 volumeDims;


void main() {

    // world space: shift to volume center to origin and
    // scale positionObj by volumeDims
    vec3 positionWorld = (position - .5f) * volumeDims;
    gl_Position = projView * vec4(positionWorld, 1.f);

    // get rayOrigCube and rayDirCube, in [0, 1]^3 cube space
//    rayOrigCube = camPosWorld / volumeDims + .5f;
//    rayDirCube = positionCube - rayOrigCube;

    positionCube = position;

    // DEBUG
//    gl_Position = vec4(0.f, 0.f, 0.f, 1.f);
//    gl_Position = vec4(positionObj, 1.f);
}
