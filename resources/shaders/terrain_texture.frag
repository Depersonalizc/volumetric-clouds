#version 330 core

// Create a UV coordinate in variable
in vec2 uv;

// Add a sampler2D uniform
//layout (binding = 2)
uniform sampler2D depth_sampler;
//layout (binding = 3)
uniform sampler2D color_sampler;



uniform float near; // near-plane and far-plane
uniform float far;

out vec4 fragColor;

//float near_plane = 0.01f;
//float far_plane = 100.f;

float linearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC, [0, 1] -> [-1, 1]
    return (2.0 * near * far) / (far + near - z * (far - near));  // linearize, [-1, 1] -> [near, far]
}

void main()
{
    float depth = texture(depth_sampler, uv).r;
    float linearDepth = linearizeDepth(depth);

//    fragColor = vec4(vec3(linearDepth), 1);
    fragColor = texture(color_sampler, uv);

}
