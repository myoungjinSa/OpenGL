#version 400 core

layout (location = 0) in vec3 inputPosition;
layout (location = 2) in vec2 inputTexcoord;

uniform mat4 worldMatrix;

out VS_OUT
{
	vec2 texcoord;
} vs_out;

void main(void){
	gl_Position = worldMatrix * vec4(inputPosition, 1.0);

	vs_out.texcoord = inputTexcoord;
}