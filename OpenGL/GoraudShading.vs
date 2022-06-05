#version 400 core

layout (location = 0) in vec3 inputPosition;
layout (location = 1) in vec4 inputColor;
layout (location = 2) in vec2 inputTexCoord;
layout (location = 3) in vec3 inputNormal;

uniform mat4 worldViewMatrix;
uniform mat4 projectionMatrix;
uniform mat3 normalMatrix;

uniform vec3 lightPosition;

uniform vec3 diffuseColor;
uniform vec3 specularColor;
uniform vec3 ambientColor;
uniform float shininess = 16.0;


out VS_OUT{
	vec2 uv;
	vec3 color;
} vs_out;

void main(){
	vec4 vertPosition4 = worldViewMatrix * vec4(inputPosition, 1.0);
	vec3 normal = normalMatrix * inputNormal;

	vec3 lightDir = lightPosition - vertPosition4.xyz;
	vec3 viewDir = -vertPosition4.xyz;

	normal = normalize(normal);
	lightDir = normalize(lightDir);
	viewDir = normalize(viewDir);

	vec3 reflection = reflect(-lightDir, normal);

	vec3 diffuse = max(dot(normal, lightDir), 0.0) * diffuseColor;
	vec3 specular = pow(max(dot(normal, lightDir), 0.0), shininess) * specularColor;

	vs_out.uv = vec2(inputTexCoord.x, 1 - inputTexCoord.y);
	vs_out.color = ambientColor + diffuse + specular;

	gl_Position = projectionMatrix * vertPosition4;
}