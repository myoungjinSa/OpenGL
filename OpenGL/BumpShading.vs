#version 400 core

layout (location = 0) in vec3 inputPosition;
layout (location = 1) in vec4 inputColor;
layout (location = 2) in vec2 inputTexcoord;
layout (location = 3) in vec3 inputNormal;

uniform mat4 worldMatrix;
uniform mat4 viewMatrix;

out VS_OUT
{
	vec3 normal;
	vec2 texcoord;

} vs_out;

void main(void){
	gl_Position = worldMatrix * vec4(inputPosition, 1.0);

	vs_out.normal = inputNormal;
	vs_out.texcoord = inputTexcoord;
}