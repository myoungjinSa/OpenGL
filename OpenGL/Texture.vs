#version 400 core

layout (location = 0) in vec3 inputPosition;
layout (location = 1) in vec4 inputColor;
layout (location = 2) in vec2 inputTexCoord;

uniform mat4 worldMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out VS_OUT{
    vec4 COLOR;
	vec2 TEXCOORD;
} vs_out;


void main(){
	gl_Position = projectionMatrix * viewMatrix * worldMatrix * vec4(inputPosition, 1.0);

	vs_out.COLOR = inputColor;
	vs_out.TEXCOORD = vec2(inputTexCoord.x, 1 - inputTexCoord.y);
}