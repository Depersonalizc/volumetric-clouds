#version 330 core
in vec2 vert;
in vec4 sample_norm;
in vec3 lightDir;
in vec2 uv;

uniform sampler2D color_sampler;

out vec4 fragColor;

float nonNeg(float height) {
    return (height + 1) / 2;
}

void main(void)
{
    vec3 sampleColor = texture(color_sampler, uv).rgb;
    fragColor = vec4((clamp(dot(normalize(sample_norm.xyz), lightDir), 0, 1)*0.7 + 0.3)* sampleColor, 1.0);

//// DEBUG

//    fragColor = vec4(sampleColor, 1); // test sample color
//    fragColor = vec4(sampleNorm.xyz, 1); // test sample norm

}
