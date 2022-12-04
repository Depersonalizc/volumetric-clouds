#version 330 core

// Create a UV coordinate in variable
in vec2 uv;

// Add a sampler2D uniform
uniform sampler2D sampler_texture;



uniform float w; // Width and height of the image, used to calculate offset in filter masks
uniform float h;

out vec4 fragColor;


void main()
{
    //fragColor = vec4(1);
    // Set fragColor using the sampler2D at the UV coordinate
    fragColor = texture(sampler_texture, uv);
    fragColor = vec4(1.0, 0.0, 0.0, 1.0);

}
