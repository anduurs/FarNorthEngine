#version 330 core

layout (location = 0) in vec3 attribute_position;
layout (location = 1) in vec3 attribute_normal;
layout (location = 2) in vec2 attribute_textureCoord;

out vs_data
{
	vec3 position;
	vec3 normal;
	vec2 textureCoord;
	vec3 cameraViewDirection;
} vs_out;

uniform mat4 localToWorldMatrix;
uniform mat4 cameraViewMatrix;
uniform mat4 projectionMatrix;

void main()
{
	vec4 worldSpacePosition = localToWorldMatrix * vec4(attribute_position, 1.0);
	vec4 viewSpacePosition = cameraViewMatrix * worldSpacePosition;

	vs_out.position = worldSpacePosition.xyz;
	vs_out.normal = mat3(transpose(inverse(localToWorldMatrix))) * attribute_normal;
	vs_out.textureCoord = attribute_textureCoord;
	vs_out.cameraViewDirection = normalize((inverse(cameraViewMatrix) * vec4(0.0,0.0,0.0,1.0)).xyz - worldSpacePosition.xyz);

	gl_Position = projectionMatrix * viewSpacePosition;
}