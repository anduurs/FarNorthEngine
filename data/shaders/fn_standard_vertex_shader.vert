#version 330 core

layout (location = 0) in vec3 attribute_position;
layout (location = 1) in vec3 attribute_normal;
layout (location = 2) in vec2 attribute_textureCoord;
layout (location = 3) in vec3 attribute_tangent;

out vs_data
{
	vec3 position;
	vec2 textureCoord;
	vec3 cameraViewPosition;
    vec3 lightDirection;
} vs_out;

uniform mat4 localToWorldMatrix;
uniform mat4 cameraViewMatrix;
uniform mat4 projectionMatrix;

uniform vec3 lightDirection;

void main()
{
	vec4 worldSpacePosition = localToWorldMatrix * vec4(attribute_position, 1.0);
	vec4 viewSpacePosition = cameraViewMatrix * worldSpacePosition;

	vec3 T = normalize(vec3(localToWorldMatrix * vec4(attribute_tangent, 0.0)));
	vec3 N = normalize(vec3(localToWorldMatrix * vec4(attribute_normal, 0.0)));

	T = normalize(T - dot(T, N) * N);

	vec3 B = cross(N, T);

	mat3 TBN = transpose(mat3(T, B, N));

	vs_out.position = TBN * worldSpacePosition.xyz;
	vs_out.textureCoord = attribute_textureCoord;
	vs_out.cameraViewPosition = TBN * ((inverse(cameraViewMatrix) * vec4(0.0,0.0,0.0,1.0)).xyz - worldSpacePosition.xyz);
	vs_out.lightDirection = TBN * lightDirection;

	gl_Position = projectionMatrix * viewSpacePosition;
}