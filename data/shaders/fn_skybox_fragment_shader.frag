#version 330 core

layout (location = 0) out vec4 fragColor;

in vec3 textureCoords;

uniform samplerCube skyboxTexture;

void main()
{
	fragColor = texture(skyboxTexture, textureCoords);
}