#version 330 core

layout (location = 0) in vec2 attribute_position;
layout (location = 1) in vec2 attribute_textureCoords;

out vec2 textureCoords;

void main()
{
	textureCoords = attribute_textureCoords;
	gl_Position = vec4(attribute_position.x, attribute_position.y, 0.0f, 1.0f);
}