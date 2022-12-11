#version 330 core
in vec3 vert;
in vec4 norm;
in vec4 sample_norm;
in vec3 color;
in vec3 lightDir;
//in vec2 uv;

uniform bool wireshade;

// Add a sampler2D uniform
//layout (binding = 3)
uniform sampler2D color_sampler;
//layout (binding = 4)
uniform sampler2D height_sampler;
//layout (binding = 5)
uniform sampler2D normal_sampler;

out vec4 fragColor;

float nonNeg(float height) {
    return (height + 1) / 2;
}

void main(void)
{
    if (wireshade) {
        fragColor = vec4(color,1);
    } else {
        vec3 objColor = color;
        vec2 uv = vec2(vert.z-floor(vert.z), vert.x-floor(vert.x));

        float cur_height = texture(height_sampler, uv).r;
        vec3 cur_color = texture(color_sampler, uv).rgb;

        fragColor = vec4((clamp(dot(norm.xyz, lightDir), 0, 1)*0.7 + 0.3)* cur_color, 1.0);


        // per-vertex
//        fragColor = vec4(vec3(norm.xyz), 1);  // normal
//        fragColor = vec4(vec3( nonNeg(vert.y)), 1);  // height

        // height/normal map sampling
        vec3 sample_norm3 = normalize(sample_norm.xyz);
        fragColor = vec4(sample_norm3, 1);  // normal
//        fragColor = vec4(vec3( nonNeg(texture(height_sampler, uv).r) ), 1);  // height
//        fragColor = vec4((clamp(dot(sample_norm3, lightDir), 0, 1)*0.7 + 0.3)* cur_color, 1.0);  // shading

    //    fragColor = vec4(1,0,0,1);
//        fragColor = vec4(vec2(vert.xz), 0, 1);
//        fragColor = vec4(vec3(sample_norm.xyz), 1);
//        fragColor = vec4(cur_color,1);
    }
}
