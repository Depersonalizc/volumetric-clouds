#version 330 core
layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 inColor;
out vec4 vert;
out vec4 norm;
out vec3 color;
out vec3 lightDir;

uniform mat4 projMatrix;
uniform mat4 mvMatrix;

void main()
{
    vert  = mvMatrix * vec4(vertex, 1.0);
    norm  = transpose(inverse(mvMatrix)) *  vec4(normal, 0.0);
    color = inColor;
    lightDir = normalize(vec3(mvMatrix * vec4(1, 0, 1, 0)));
    gl_Position = projMatrix * mvMatrix * vec4(vertex, 1.0);
}
