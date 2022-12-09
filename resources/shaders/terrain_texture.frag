#version 330 core

// Create a UV coordinate in variable
in vec2 uv;

// Add a sampler2D uniform
//layout (binding = 2)
uniform sampler2D depth_sampler;
//layout (binding = 3)
uniform sampler2D color_sampler;
//layout (binding = 4)
uniform sampler2D height_sampler;
//layout (binding = 5)
uniform sampler2D normal_sampler;


uniform float near; // near-plane and far-plane
uniform float far;

out vec4 fragColor;

//float near_plane = 0.01f;
//float far_plane = 100.f;

float linearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC
    return (2.0 * near * far) / (far + near - z * (far - near));
}

void main()
{
    float depth = texture(depth_sampler, uv).r;
    float linearDepth = linearizeDepth(depth);
//    fragColor = vec4(vec3(linearDepth), 1);

//    fragColor = texture(color_sampler, uv); // test for color texture
//    fragColor = vec4(vec3(texture(height_sampler, uv).r*5+0.1), 1); // test for height map, scale and offset for testing
//    fragColor = vec4(vec3(texture(normal_sampler, uv).r),1);
       fragColor = vec4(texture(normal_sampler, uv).rgb,1);
//    fragColor = vec4(1,0,0,1);
}
