#version 430 core
layout(location = 0) in vec2 vertex;

out vec4 vert;
out vec4 sample_norm;
out vec3 lightDir;
out vec2 uv;

uniform float terrainNoiseScaling = 1.f;
uniform mat4 projViewMatrix;
uniform mat4 transInvViewMatrix;

uniform sampler2D height_sampler;
uniform sampler2D normal_sampler;

void main()
{
    vec2 uv = vertex.yx;
    uv *= terrainNoiseScaling;
    uv = fract(uv);

    lightDir = normalize(vec3(1.0,0.0,1.0));

    // sample and pass norm to fragment shader
    sample_norm  = transInvViewMatrix * vec4(texture(normal_sampler, uv).rgb, 0.0);

    // height map sampling
    float height = texture(height_sampler, uv).r / terrainNoiseScaling / 3;
    vec3 pos = vec3(vertex.x, height, vertex.y);
    gl_Position = projViewMatrix * vec4(pos, 1.0);
//    gl_Position = projViewMatrix * vec4(vec3(vertex.x, 0, vertex.y),1);

//// DEBUG
//    pos[1] = 0;
//    gl_Position = projViewMatrix * vec4(pos, 1.0); // to flat for seamless checking
}
