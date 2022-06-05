#version 400 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

out vec3 normal;
out vec4 color;
out vec2 texcoord;
out vec3 currentPos;
out vec3 lightPosition;
out vec3 cameraPosition;

in VS_OUT
{
	vec3 normal;
	vec2 texcoord;

	mat4 worldMatrix;
	mat4 worldViewMatrix;
	mat4 projectionMatrix;

	vec3 lightPos;
	vec3 cameraPos;
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

	vec3 T = normalize(vec3(data_in[0].worldMatrix * vec4(tangent, 0.0f)));
	vec3 B = normalize(vec3(data_in[0].worldMatrix * vec4(bitangent, 0.0f)));
	vec3 N = normalize(vec3(data_in[0].worldMatrix * vec4(cross(edge1, edge0), 0.0f)));

	mat3 TBN = mat3(T, B, N);

	//TBN is an orthogonal matrix and so its inverse is equal to its transpose
	TBN = transpose(TBN);

	gl_Position = data_in[0].projectionMatrix * gl_in[0].gl_Position;

	normal = data_in[0].normal;
	texcoord = data_in[0].texcoord;

	//Change all lighting variables to TBN space
	currentPos = TBN * gl_in[0].gl_Position.xyz;
	lightPosition = TBN * data_in[0].lightPos;
	cameraPosition = TBN * data_in[0].cameraPos;
	EmitVertex();

	
	gl_Position = data_in[1].projectionMatrix * gl_in[1].gl_Position;

	normal = data_in[1].normal;
	texcoord = data_in[1].texcoord;

	//Change all lighting variables to TBN space
	currentPos = TBN * gl_in[1].gl_Position.xyz;
	lightPosition = TBN * data_in[1].lightPos;
	cameraPosition = TBN * data_in[1].cameraPos;
	EmitVertex();

		
	gl_Position = data_in[2].projectionMatrix * gl_in[2].gl_Position;

	normal = data_in[2].normal;
	texcoord = data_in[2].texcoord;

	//Change all lighting variables to TBN space
	currentPos = TBN * gl_in[2].gl_Position.xyz;
	lightPosition = TBN * data_in[2].lightPos;
	cameraPosition = TBN * data_in[2].cameraPos;
	EmitVertex();

	EndPrimitive();
}