#version 400 core

layout (location = 0) in vec3 inputPosition;
layout (location = 1) in vec4 inputColor;
layout (location = 2) in vec2 inputTexCoord;
layout (location = 3) in vec3 inputNormal;

uniform mat4 worldMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform vec3 lightPosition;

varying vec3 posInEyeCoord;

out VS_OUT {
	vec2 TEXCOORD;
	vec3 N;
	vec3 L;
	vec3 V;
} vs_out;

void main(){
	vec4 vertPosition4 = viewMatrix * worldMatrix * vec4(inputPosition, 1.0);
	posInEyeCoord = vec3(vertPosition4) / vertPosition4.w;
	
	gl_Position = projectionMatrix * vertPosition4;
	
	vs_out.TEXCOORD = vec2(inputTexCoord.x, 1- inputTexCoord.y);
	vs_out.N = normalize(mat3(worldMatrix) * inputNormal);
	vs_out.L = normalize(lightPosition - posInEyeCoord);
	vs_out.V = normalize(-posInEyeCoord);
}