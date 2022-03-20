#version 400 core
layout (location = 0) in vec3 inputPosition;

out vec3 texCoords;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main()
{
	vec4 position = projectionMatrix * viewMatrix * vec4(inputPosition, 1.0f);
	gl_Position = vec4(position.x, position.y, position.w, position.w);		
	texCoords = vec3(inputPosition.x, inputPosition.y, -inputPosition.z);
}