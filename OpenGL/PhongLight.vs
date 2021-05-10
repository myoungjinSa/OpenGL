#version 400 core

layout (location = 0) in vec3 inputPosition;
layout (location = 1) in vec2 inputTexCoord;
layout (location = 2) in vec3 inputNormal;

uniform mat4 worldMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

uniform vec3 lightDirection;

out VS_OUT {
	vec2 TEXCOORD;
	vec3 N;
	vec3 L;
	vec3 V;
} vs_out;

void main(){
	mat4 worldViewMatrix = viewMatrix * worldMatrix;
	vec4 P = worldViewMatrix * vec4(inputPosition, 1.0);

	gl_Position = projectionMatrix * P;

	vs_out.TEXCOORD = inputTexCoord;
	vs_out.N = mat3(worldViewMatrix) * inputNormal;
	vs_out.L = lightDirection - P.xyz;
	vs_out.V = -P.xyz;
}