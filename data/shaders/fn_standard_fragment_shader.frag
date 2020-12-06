#version 330 core

layout (location = 0) out vec4 fragColor;

in vec2 texture_coord;

uniform sampler2D textureColor;

void main()
{
    fragColor = texture(textureColor, texture_coord);
} 