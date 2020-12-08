#version 330 core

layout (location = 0) out vec4 fragColor;

in vec2 textureCoords;

uniform sampler2D sceneTexture;

void main()
{
	//fragColor = vec4(vec3(1.0 - texture(sceneTexture, textureCoords)), 1.0);
	fragColor = texture(sceneTexture, textureCoords);
}