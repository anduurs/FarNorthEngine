#version 330 core

layout (location = 0) out vec4 fragColor;

in vec2 texture_coord;

uniform sampler2D textureColor;

void main()
{
    fragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
    //fragColor = texture(textureColor, texture_coord);
} 