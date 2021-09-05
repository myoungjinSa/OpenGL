#version 400 core

layout (location = 0) in vec3 inputPosition;
layout (location = 1) in vec4 inputColor;
layout (location = 2) in vec2 inputTexCoord;
layout (location = 3) in vec3 inputNormal;

uniform mat4 worldMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform vec3 lightPosition;
uniform vec3 worldCameraPosition;

out VS_OUT {
	vec2 TEXCOORD;
	vec3 N;
	vec3 L;
	vec3 V;
} vs_out;

void main(){
	gl_Position = projectionMatrix * viewMatrix * worldMatrix * vec4(inputPosition, 1.0);

	vec4 worldPosition = worldMatrix * vec4(inputPosition, 1.0);

	vs_out.TEXCOORD = inputTexCoord;
	vs_out.N = mat3(worldMatrix) * inputNormal;
	vs_out.L = lightPosition - worldPosition.xyz;
	vs_out.V = worldCameraPosition - worldPosition.xyz;
}