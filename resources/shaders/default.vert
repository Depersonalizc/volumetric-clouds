#version 430 core

//layout(location = 0) in vec3 position;  // cube [0, 1]^3

//out vec3 positionWorld;

layout (location = 0) in vec3 position;  // screen quad
layout (location = 1) in vec2 attribUV;

out vec3 rayDirWorldspace;  // direction of vertex on focal plane f=1 in world space

uniform mat4 projView;

// added
uniform float xMax, yMax;  // rayDirWorldspace lies within [-xMax, xMax] x [-yMax, yMax] x {1.0}
uniform mat4 viewInverse;
uniform vec3 rayOrigWorld;

// volume transforms
uniform vec3 volumeScaling;
uniform vec3 volumeTranslate;


void main() {

//    positionWorld = position * volumeScaling + volumeTranslate;
//    gl_Position = projView * vec4(positionWorld, 1.f);

    vec4 rayDirCameraspace = vec4(position.x * xMax, position.y * yMax, -1.f, 0.f);

    rayDirWorldspace = vec3(viewInverse * rayDirCameraspace);

    // DEBUG
//    rayDirWorldspace = vec3(position);

    gl_Position = vec4(position, 1.f);  // quad position


    // DEBUG
//    gl_Position = vec4(0.f, 0.f, 0.f, 1.f);
//    gl_Position = vec4(positionObj, 1.f);
}
