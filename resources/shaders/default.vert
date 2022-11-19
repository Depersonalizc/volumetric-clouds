#version 430 core

layout(location = 0) in vec3 position;  // cube [0, 1]^3

out vec3 positionWorld;

uniform mat4 projView;

// volume transforms
uniform vec3 volumeScaling;
uniform vec3 volumeTranslate;


void main() {

    positionWorld = position * volumeScaling + volumeTranslate;
    gl_Position = projView * vec4(positionWorld, 1.f);

    // DEBUG
//    gl_Position = vec4(0.f, 0.f, 0.f, 1.f);
//    gl_Position = vec4(positionObj, 1.f);
}
