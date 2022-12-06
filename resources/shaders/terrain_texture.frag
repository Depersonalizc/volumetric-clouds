#version 330 core

// Create a UV coordinate in variable
in vec2 uv;

// Add a sampler2D uniform
uniform sampler2D texture_sampler;



uniform float w; // Width and height of the image, used to calculate offset in filter masks
uniform float h;

out vec4 fragColor;

float near_plane = 0.01f;
float far_plane = 100.f;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC
    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));
}

void main()
{
    float depth = texture(texture_sampler, uv).r;
    float linear_depth = LinearizeDepth(depth);
    fragColor = vec4(linear_depth, linear_depth, linear_depth, 1);
}
