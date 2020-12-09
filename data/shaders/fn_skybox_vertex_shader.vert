#version 330 core

layout (location = 0) in vec3 attribute_position;

out vec3 textureCoords;

uniform mat4 cameraViewMatrix;
uniform mat4 projectionMatrix;

void main()
{
	textureCoords = attribute_position;
	vec4 pos = projectionMatrix * mat4(mat3(cameraViewMatrix)) * vec4(attribute_position, 1.0);
	gl_Position = pos.xyww;
}