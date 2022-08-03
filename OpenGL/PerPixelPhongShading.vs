#version 400 core

layout (location = 0) in vec3 inputPosition;
layout (location = 2) in vec2 inputTexCoord;
layout (location = 3) in vec3 inputNormal;

uniform mat4 worldMatrix;
uniform mat4 worldViewMatrix;
uniform mat4 projectionMatrix;
uniform mat3 normalMatrix;		

out VS_OUT {
	vec4 worldPosition;			//position of the vertex(and fragment) in world Space
	vec3 varyingNormalDirection;		//surface normal vector in world space
	vec2 UV;
} vs_out;

void main(){
	vs_out.worldPosition = worldMatrix * vec4(inputPosition, 1.0f);	
	vs_out.varyingNormalDirection = normalize(normalMatrix * inputNormal);

	vs_out.UV = vec2(inputTexCoord.x, 1- inputTexCoord.y);

	gl_Position = projectionMatrix * worldViewMatrix * vec4(inputPosition, 1.0f);
}