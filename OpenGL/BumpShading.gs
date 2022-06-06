#version 400 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

uniform mat4 worldMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform vec3 lightPosition;
uniform vec3 cameraPosition;

out vec2 texcoord;
out vec3 lightDir;
out vec3 eyeDir;

in VS_OUT
{
	vec3 normal;
	vec2 texcoord;
} data_in[];

void main(){
	//edges of the triangle;
	vec3 edge0 = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
	vec3 edge1 = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;

	//lengths of UV differences
	vec2 deltaUV0 = data_in[1].texcoord - data_in[0].texcoord;
	vec2 deltaUV1 = data_in[2].texcoord - data_in[0].texcoord;

	//one over the determinant
	float invDet = 1.0f / (deltaUV0.x * deltaUV1.y - deltaUV1.x * deltaUV0.y);

	vec3 tangent = vec3(invDet * (deltaUV1.y * edge0 - deltaUV0.y * edge1));
	vec3 bitangent = vec3(invDet * (-deltaUV1.x * edge0 + deltaUV0.x * edge1));

	vec3 T = normalize(vec3(worldMatrix * vec4(tangent, 0.0f)));
	vec3 B = normalize(vec3(worldMatrix * vec4(bitangent, 0.0f)));
	vec3 N = normalize(vec3(worldMatrix * vec4(cross(edge0, edge1), 0.0f)));

	mat3 TBN = mat3(T, B, N);

	//TBN is an orthogonal matrix and so its inverse is equal to its transpose
	TBN = transpose(TBN);

	texcoord = data_in[0].texcoord;

	//Change all lighting variables to TBN space
	vec3 lightDirection = lightPosition - gl_in[0].gl_Position.xyz;

	lightDir = normalize(TBN * lightDirection);
	vec3 viewDir = cameraPosition - gl_in[0].gl_Position.xyz;
	eyeDir =  normalize(TBN * viewDir);
	gl_Position = projectionMatrix * viewMatrix * gl_in[0].gl_Position;

	EmitVertex();

	
	texcoord = data_in[1].texcoord;

	//Change all lighting variables to TBN space
	lightDirection = lightPosition - gl_in[1].gl_Position.xyz;
	lightDir = normalize(TBN * lightDirection);
	viewDir = cameraPosition - gl_in[1].gl_Position.xyz;
	eyeDir =  normalize(TBN * viewDir);

	gl_Position = projectionMatrix * viewMatrix * gl_in[1].gl_Position;

	EmitVertex();

	texcoord = data_in[2].texcoord;

	//Change all lighting variables to TBN space
	lightDirection = lightPosition - gl_in[2].gl_Position.xyz;
	lightDir =  normalize(TBN * lightDirection);
	viewDir = cameraPosition - gl_in[2].gl_Position.xyz;
	eyeDir = normalize(TBN * viewDir);
	
	gl_Position = projectionMatrix * viewMatrix * gl_in[2].gl_Position;

	EmitVertex();

	EndPrimitive();
}