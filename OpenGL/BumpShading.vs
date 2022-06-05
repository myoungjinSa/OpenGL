#version 400 core

layout (location = 0) in vec3 inputPosition;
layout (location = 1) in vec4 inputColor;
layout (location = 2) in vec2 inputTexcoord;
layout (location = 3) in vec3 inputNormal;

out VS_OUT
{
	vec2 texcoord;
	vec3 eyeDir;
	vec3 lightDir;
} vs_out;

uniform mat4 worldViewMatrix;
uniform mat4 projectionMatrix;

uniform vec3 lightPosition;

void main(void){
	vec4 vertPosition4 = worldViewMatrix * vec4(inputPosition, 1.0);

	vec3 normal = normalize(mat3(worldViewMatrix) * inputNormal);
	vec3 tangent = normalize(mat3(worldViewMatrix) * inputTangent);
	
	vec3 bitangent = cross(normal, tangent);

	vec3 lightDir = lightPosition - vertPosition4.xyz;
	vec3 viewDir = -vertPosition4.xyz;
	vs_out.lightDir = normalize(vec3(dot(viewDir, tangent), dot(viewDir, bitangent), dot(viewDir, normal));
	vs_out.eyeDir = normalize(vec3(dot(viewDir, tangent), dot(viewDir, bitangent), dot(viewDir, normal));

	vs_out.texcoord = inputTexcoord;

	gl_Position = projectionMatrix * vertPosition4;
}