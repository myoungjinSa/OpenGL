#version 400 core

layout (location = 0) in vec3 inputPosition;
layout (location = 1) in vec4 inputColor;
layout (location = 2) in vec2 inputTexcoord;
layout (location = 3) in vec3 inputNormal;

uniform mat4 worldMatrix;
uniform mat4 worldViewMatrix;
uniform mat4 projectionMatrix;

uniform vec3 cameraPosition;
uniform vec3 lightPosition;

out VS_OUT
{
	vec3 normal;
	vec2 texcoord;

	mat4 worldMatrix;
	mat4 worldViewMatrix;
	mat4 projectionMatrix;
	vec3 lightPos;
	vec3 cameraPos;
} vs_out;

void main(void){
	gl_Position = worldViewMatrix * vec4(inputPosition, 1.0);

	vs_out.normal = inputNormal;
	vs_out.texcoord = inputTexcoord;
	vs_out.worldMatrix = worldMatrix;
	vs_out.worldViewMatrix = worldViewMatrix;
	vs_out.projectionMatrix = projectionMatrix;
	vs_out.lightPos = lightPosition;
	vs_out.cameraPos = cameraPosition;
}