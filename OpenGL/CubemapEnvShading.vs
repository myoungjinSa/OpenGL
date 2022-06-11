
#version 400 core

layout (location = 0) in vec3 inputPosition;
layout (location = 2) in vec2 inputTexcoord;
layout (location = 3) in vec3 inputNormal;

out VS_OUT{
	vec3 passNormal;
	vec2 passTexcoords;
	vec3 reflectedVector;
	vec3 refractedVector;
	vec3 viewDir;
}vs_out;

uniform mat4 worldMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform vec3 cameraPosition;
uniform vec3 lightPosition;

varying vec3 lightDirection;

void main(void){
	vec4 worldPosition = worldMatrix * vec4(inputPosition, 1.0);
	gl_Position = projectionMatrix * viewMatrix * worldPosition;

	lightDirection = normalize(lightPosition - worldPosition.xyz);
	
	vs_out.passTexcoords = inputTexcoord;
	vs_out.passNormal = inputNormal;
	vec3 unitNormal = normalize(inputNormal);

	vs_out.viewDir = normalize(worldPosition.xyz - cameraPosition);
	vs_out.reflectedVector = reflect(vs_out.viewDir, unitNormal);
	vs_out.refractedVector = refract(vs_out.viewDir, unitNormal, 1.0 / 1.33);
}