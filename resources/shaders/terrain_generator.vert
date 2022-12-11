#version 330 core
layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 inColor;
out vec3 vert;
out vec4 norm;
out vec4 sample_norm;
out vec3 color;
out vec3 lightDir;

uniform mat4 projViewMatrix;
//uniform mat4 mvMatrix;
uniform mat4 transInvViewMatrix;

uniform sampler2D height_sampler;
uniform sampler2D normal_sampler;

void main()
{
//    vert  = mvMatrix * vec4(vertex, 1.0);
//    norm  = transpose(inverse(mvMatrix)) *  vec4(normal, 0.0);
//    lightDir = normalize(vec3(mvMatrix * vec4(1, 0, 1, 0)));
//    gl_Position = projMatrix * mvMatrix * vec4(vertex, 1.0);

    vert = vertex;
    color = inColor;
    norm = transInvViewMatrix * vec4(normal, 0.0);
    vec2 uv = vec2(vert.z-floor(vert.z), vert.x-floor(vert.x));
//    vec3 temp_norm = normalize(texture(normal_sampler, uv).rgb*2.f-1.f);
    sample_norm  = transInvViewMatrix * vec4(texture(normal_sampler, uv).rgb, 0.0);
    lightDir = normalize(vec3(1.0,0.0,1.0));


    // height map sampling
    float height = texture(height_sampler, uv).r;
//    vec3 pos = vec3(vertex.x, height, vertex.z);
    vec3 pos = vec3(vertex.x, 0, vertex.z);
    gl_Position = projViewMatrix * vec4(pos, 1.0);


    // directly passed-in vertex position
//    gl_Position = projViewMatrix * vec4(vertex, 1.0);

}
