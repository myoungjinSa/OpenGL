#version 400 core
layout (location = 0) in vec3 inputPosition;
layout (location = 1) in vec4 inputColor;
layout (location = 2) in vec2 inputTexCoord;
layout (location = 3) in vec3 inputNormal;


uniform mat4 worldMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform vec3 lightPosition;
uniform vec3 worldCameraPosition;

uniform float shininess = 64.0;
uniform float Ka;			//Ambient reflection coefficient
uniform float Kd;			//Diffuse reflection coefficient
uniform float Ks;			//Specular reflection coefficient


uniform vec3 diffuseColor;
uniform vec3 specularColor;
uniform vec3 ambientColor;

uniform float shininessValue;


varying vec3 vertPos;
varying vec3 normalInterp;
varying vec4 color;

void main(){
	vec4 vertPosition4 = viewMatrix * worldMatrix * vec4(inputPosition, 1.0);
	vertPos = vec3(vertPosition4) / vertPosition4.w;

	normalInterp = mat3(worldMatrix) * inputNormal;

	gl_Position = projectionMatrix * vertPosition4;

	vec3 N = normalize(normalInterp);
	vec3 L = normalize(lightPosition - vertPos);

	//Lambert's cosine law
	float lambertian = max(dot(N, L), 0.0);
	float specular = 0.0;
	if(0.0 < lambertian){
		vec3 R = reflect(-L, N);
		vec3 V = normalize(-vertPos);

		//Compute the specular term
		float specAngle = max(dot(R, V), 0.0);
		specular = pow(specAngle, shininessValue);
	}

	color = vec4(Ka * ambientColor + Kd * lambertian * diffuseColor + Ks * specular * specularColor, 1.0);
}