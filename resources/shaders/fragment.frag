#version 330 core
in vec4 vert;
in vec4 norm;
in vec3 color;
in vec3 lightDir;

uniform bool wireshade;

out  vec4 fragColor;

void main(void)
{
    if (wireshade) {
        fragColor = vec4(color,1);
    } else {
        vec3 objColor = color;
        fragColor = vec4((clamp(dot(norm.xyz, lightDir), 0, 1) * 0.7 +  0.3) * objColor, 1.0) + 0.5*vec4(0.0, 0.2, 0.0, 1.0);
        fragColor = vec4(clamp(fragColor[0], 0.f, 1.f), clamp(fragColor[1], 0.f, 1.f), clamp(fragColor[2], 0.f, 1.f), 1.0);
    }

    //fragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
