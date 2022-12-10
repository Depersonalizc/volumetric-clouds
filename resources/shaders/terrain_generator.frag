#version 330 core
in vec3 vert;
in vec4 norm;
in vec3 color;
in vec3 lightDir;
//in vec2 uv;

uniform bool wireshade;

// Add a sampler2D uniform
//layout (binding = 2)
uniform sampler2D height_sampler;
//layout (binding = 3)
uniform sampler2D normal_sampler;

out vec4 fragColor;

void main(void)
{
    if (wireshade) {
        fragColor = vec4(color,1);
    } else {
        vec3 objColor = color;
        vec2 uv = vec2(vert.z, vert.x);
        vec3 cur_norm = texture(normal_sampler, uv).rgb;
        float cur_height = texture(height_sampler, uv).r;
        fragColor = vec4((clamp(dot(norm.xyz, lightDir), 0, 1)*0.7 + 0.3)* objColor, 1.0);
        fragColor = vec4((clamp(dot(cur_norm, lightDir), 0, 1)*0.7 + 0.3)* objColor, 1.0);
    //    fragColor = vec4(1,0,0,1);
//        fragColor = vec4(vec2(vert.xz), 0, 1);
        fragColor = vec4(vec3(norm.xyz), 1);
//        fragColor = vec4(vec3(cur_norm), 1);
        fragColor = vec4(vec3(cur_height*5+0.1),1);
//        fragColor = vec4(vec3(0),1);
//        fragColor = vec4(vec3(vert.y), 1);
    }
}
