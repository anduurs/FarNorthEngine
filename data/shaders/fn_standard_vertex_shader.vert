#version 330 core

layout (location = 0) in vec3 attribute_position;
layout (location = 1) in vec3 attribute_normal;
layout (location = 2) in vec2 attribute_textureCoord;

out vec2 texture_coord;

uniform mat4 localToWorldMatrix;
uniform mat4 cameraViewMatrix;
uniform mat4 projectionMatrix;

void main()
{
	vec4 worldSpacePosition = localToWorldMatrix * vec4(attribute_position, 1.0);
	vec4 viewSpacePosition = cameraViewMatrix * worldSpacePosition;

	gl_Position = projectionMatrix * viewSpacePosition;

	texture_coord = attribute_textureCoord;
}