#version 400 core

layout (location = 0) in vec3 inputPosition;
layout (location = 1) in vec4 inputColor;
layout (location = 2) in vec2 inputTexCoord;
layout (location = 3) in vec3 inputNormal;

uniform mat4 worldViewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 normalMatrix;

uniform vec3 lightPosition;

varying vec3 posInEyeCoord;

out VS_OUT {
	vec3 V;
	vec2 UV;
	vec3 N;
	vec3 L;
} vs_out;

void main(){
	vec4 vertPosition4 = worldViewMatrix * vec4(inputPosition, 1.0);
	posInEyeCoord = vec3(vertPosition4) / vertPosition4.w;
	
	gl_Position = projectionMatrix * vertPosition4;
	
	vs_out.V = normalize(-posInEyeCoord);
	vs_out.UV = vec2(inputTexCoord.x, 1- inputTexCoord.y);
	vs_out.N = normalize(vec3(normalMatrix * vec4(inputNormal, 0.0)));
	vs_out.L = normalize(lightPosition - posInEyeCoord);
}