#version 330 core

layout (location = 0) in vec3 position;

uniform mat4 localToWorldMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main()
{
	vec4 worldSpacePosition = localToWorldMatrix * vec4(position, 1.0);
	vec4 viewSpacePosition = viewMatrix * worldSpacePosition;

	gl_Position = projectionMatrix * viewSpacePosition;
}