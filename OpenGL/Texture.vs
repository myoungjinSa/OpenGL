#version 400

in vec3 inputPosition;
in vec2 inputTexCoord;
in vec3 inputNormal;

out vec3 lightDir;
out vec2 texCoord;
out vec3 normal;
out vec3 viewDir;
out vec3 reflection;

uniform mat4 worldMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform vec3 lightPosition;
uniform vec4 cameraPosition;

void main(void){
	gl_Position = worldMatrix * vec4(inputPosition, 1.0f);

	vec3 lightPos = lightPosition;
	vec3 lightDir = gl_Position.xyz - lightPos;
	lightDir = normalize(lightDir);

	vec3 viewDirection = normalize(gl_Position.xyz - cameraPosition.xyz);
	viewDir = viewDirection;

	gl_Position = viewMatrix * gl_Position;
	gl_Position = projectionMatrix * gl_Position;

	texCoord = inputTexCoord;
	normal = mat3(worldMatrix) * inputNormal;

	//Normalize the normal vector.
	normal = normalize(normal);

	reflection = reflect(lightDir, normal);
}

