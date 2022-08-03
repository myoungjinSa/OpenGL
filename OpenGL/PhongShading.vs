#version 400 core

layout (location = 0) in vec3 inputPosition;
layout (location = 2) in vec2 inputTexCoord;
layout (location = 3) in vec3 inputNormal;

uniform mat4 worldMatrix;
uniform mat4 worldViewMatrix;
uniform mat4 projectionMatrix;

uniform vec3 lightPosition;

out VS_OUT {
	vec3 V;
	vec2 UV;
	vec3 N;
	vec3 L;
} vs_out;

void main(){
	vec4 positionInWorld = worldMatrix * vec4(inputPosition, 1.0);
	vec4 positionInEye = worldViewMatrix * vec4(inputPosition, 1.0);

	gl_Position = projectionMatrix * positionInEye;
	
	vs_out.V = -positionInEye.xyz;
	vs_out.UV = vec2(inputTexCoord.x, 1- inputTexCoord.y);
	vs_out.N = mat3(worldMatrix) * inputNormal;
	vs_out.L = lightPosition - positionInWorld.xyz;
}