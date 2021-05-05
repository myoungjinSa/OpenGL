#include "OpenGL.h"
#include "Mesh.h"
#include <string>

Vertex::Vertex()
	: position()
{

}

Vertex::Vertex(const Vec3f& pos) 
	: position(pos)
{

}
Vertex::~Vertex() {

}


ColorVertex::ColorVertex() 
	: Vertex(), color()
{

}

ColorVertex::ColorVertex(const Vec3f& pos, const RGBA& _color) 
	: Vertex(pos), color(_color)
{

}


ColorVertex::~ColorVertex() {

}

void ColorVertex::Copy(const VertexMaster& source, byte* pDestination) {
	ColorVertex* pColorVertex = (ColorVertex*)(pDestination);
	pColorVertex->position = source.position;
	pColorVertex->color = source.color;
}
void ColorVertex::BindVertexBuffer(const OpenGL& gl, void* pBuffer, unsigned int vertexBufferId, unsigned int vertexCount, unsigned int sizeofVertex) {
	gl.glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	gl.glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(ColorVertex), pBuffer, GL_STATIC_DRAW);

	gl.glEnableVertexAttribArray(0);		//Vertex Position
	gl.glEnableVertexAttribArray(1);		//Vertex color.

	gl.glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	gl.glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeofVertex, 0);

	gl.glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	gl.glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, false, sizeofVertex, (unsigned char*)NULL + (3 * sizeof(float)));
}


TexturedVertex::TexturedVertex() 
	:Vertex(), uv0(), normal()
{

}
TexturedVertex::TexturedVertex(const Vec3f& pos, const Vec2f& _uv0, const Vec3f& _normal) 
	: Vertex(pos), uv0(_uv0), normal(_normal)
{

}

TexturedVertex::~TexturedVertex() {

}
void TexturedVertex::Copy(const VertexMaster& source, byte* pDestination) {
	TexturedVertex* pColorVertex = (TexturedVertex*)(pDestination);
	pColorVertex->position = source.position;
	pColorVertex->uv0 = source.uv0;
	pColorVertex->normal = source.normal;
}

void TexturedVertex::BindVertexBuffer(const OpenGL& gl, void* pBuffer, unsigned int vertexBufferId, unsigned int vertexCount, unsigned int sizeofVertex) {
	gl.glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	gl.glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(TexturedVertex), pBuffer, GL_STATIC_DRAW);

	gl.glEnableVertexAttribArray(0); // Vertex Position;
	gl.glEnableVertexAttribArray(1); // Texture coordinates;
	gl.glEnableVertexAttribArray(2); // Normal

	gl.glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	gl.glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeofVertex, 0);

	gl.glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	gl.glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeofVertex, (unsigned char*)NULL + (3 * sizeof(float)));

	gl.glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	gl.glVertexAttribPointer(2, 3, GL_FLOAT, false, sizeofVertex, (unsigned char*)NULL + (5 * sizeof(float)));

}


Mesh::Mesh()
	:vertexCount(0), vertexArrayId(0), vertexBufferId(0), indexCount(0), indexBufferId(0), drawMode(Renderer::DrawMode::TRIANGLES)
{

}

Mesh::~Mesh() {

}

bool Mesh::Initialize(const OpenGL& gl, VertexBufferBindCallback* pBindFuction, void* vertexData, unsigned int numVertices, unsigned int sizeofVertex, unsigned int* indexData, unsigned int numIndices) {
	vertexCount = numVertices;
	indexCount = numIndices;

	//Allocate an OpenGL vertex array object.
	gl.glGenVertexArrays(1, &vertexArrayId);

	//Bind the vertex array object to store all the buffers and vertex attribute we create here.
	gl.glBindVertexArray(vertexArrayId);

	//Generate an ID for the vertex buffer.
	gl.glGenBuffers(1, &vertexBufferId);
	
	//Bind the vertex buffer and load the vertex(position and color) data into the vertex buffer.
	pBindFuction(gl, vertexData, vertexBufferId, numVertices, sizeofVertex);

	gl.glGenBuffers(1, &indexBufferId);

	gl.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
	gl.glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned int), indexData, GL_STATIC_DRAW);

	return true;
	//return InitializeBuffers(gl);
}

void Mesh::Shutdown(const OpenGL& gl) {
	gl.glDisableVertexAttribArray(0);
	gl.glDisableVertexAttribArray(1);
	gl.glDisableVertexAttribArray(2);

	//Release the vertex buffer.
	gl.glBindBuffer(GL_ARRAY_BUFFER, 0);
	gl.glDeleteBuffers(1, &vertexBufferId);

	//Release the index buffer.
	gl.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	gl.glDeleteBuffers(1, &indexBufferId);

	//Release the vertex array object.
	gl.glBindVertexArray(0);
	gl.glDeleteVertexArrays(1, &vertexArrayId);
	//ShutdownBuffers(gl);
}

void Mesh::Render(const OpenGL& gl) {
	gl.glBindVertexArray(vertexArrayId);
	
	//glDrawArrays(GL_TRIANGLES, 0, vertexCount);
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
}

//bool Mesh::InitializeBuffers(const OpenGL& gl) {
//	vertexCount = 3;
//	indexCount = 3;
//
//
//	return true;
//}
//
//void Mesh::RenderBuffers(const OpenGL& gl) {
//
//}
//
//void Mesh::ShutdownBuffers(const OpenGL& gl) {
//
//}

//DiffuseMesh::DiffuseMesh() 
//	: Mesh(), vertices(nullptr), indices(nullptr)
//{
//
//}
//
//DiffuseMesh::~DiffuseMesh() {
//
//}
//
//bool DiffuseMesh::InitializeBuffers(const OpenGL& gl) {
//	vertexCount = 3;
//	indexCount = 3;
//
//
//	vertices = new ColorVertex[vertexCount];
//	if (!vertices)
//		return false;
//
//	indices = new unsigned int[indexCount];
//	if (!indices)
//		return false;
//
//	// Bottom left.
//	vertices[0].position = Vec3f(-1.0f, -1.0f, 0.0f);  // Position.
//	vertices[0].color = RGBA(0.0f, 1.0f, 0.0f, 1.0f);  // Color.
//	
//	// Top middle.
//	vertices[1].position = Vec3f(0.0f, 1.0f, 0.0f);  // Position.
//	vertices[1].color = RGBA(0.0f, 1.0f, 0.0f, 1.0f);  // Color.
//
//
//	// Bottom right.
//	vertices[2].position = Vec3f(1.0f, -1.0f, 0.0f);  // Position.
//	vertices[2].color = RGBA(0.0f, 1.0f, 0.0f, 1.0f);  // Color.
//
//	// Load the index array with data.
//	indices[0] = 0;  // Bottom left.
//	indices[1] = 1;  // Top middle.
//	indices[2] = 2;  // Bottom right.
//	
//	//Allocate an OpenGL vertex array object.
//	gl.glGenVertexArrays(1, &vertexArrayId);
//
//	//Bind the vertex array object to store all the buffers and vertex attribute we create here.
//	gl.glBindVertexArray(vertexArrayId);
//
//	//Generate an ID for the vertex buffer.
//	gl.glGenBuffers(1, &vertexBufferId);
//
//	//Bind the vertex buffer and load the vertex(position and color) data into ther vertex buffer.
//	gl.glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
//	gl.glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(ColorVertex), vertices, GL_STATIC_DRAW);
//
//	//Enable the two vertex array attribute.
//	gl.glEnableVertexAttribArray(0);		//Vertex Position
//	gl.glEnableVertexAttribArray(1);		//Vertex color.
//
//	//Specify the location and format of the position portion of the vertex buffer.
//	gl.glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
//	gl.glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(ColorVertex), 0);
//
//	//Specify the location and format of the color portion of the vertex buffer.
//	gl.glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
//	gl.glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(ColorVertex), (unsigned char*)NULL + (3 * sizeof(float)));
//
//	//Generate an ID for the index buffer
//	gl.glGenBuffers(1, &indexBufferId);
//
//	//Bind the index buffer and load the index data into it.
//	gl.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
//	gl.glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned int), indices, GL_STATIC_DRAW);
//
//
//	//Now that the buffers have been loaded we can release the array data.
//	delete[] vertices;
//	vertices = nullptr;
//
//	delete[] indices;
//	indices = nullptr;
//	return true;
//}
//
//void DiffuseMesh::RenderBuffers(const OpenGL& gl) {
//	gl.glBindVertexArray(vertexArrayId);
//
//	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
//}
//
//void DiffuseMesh::ShutdownBuffers(const OpenGL& gl) {
//	//Disable the two vertex array attributes.
//	gl.glDisableVertexAttribArray(0);
//	gl.glDisableVertexAttribArray(1);
//
//	//Release the vertex buffer.
//	gl.glBindBuffer(GL_ARRAY_BUFFER, 0);
//	gl.glDeleteBuffers(1, &vertexBufferId);
//
//	//Release the index buffer.
//	gl.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//	gl.glDeleteBuffers(1, &indexBufferId);
//
//	//Release the vertex array object.
//	gl.glBindVertexArray(0);
//	gl.glDeleteVertexArrays(1, &vertexArrayId);
//}


///////////////////////////////// Mesh Builder /////////////////////////////
MeshBuilder::MeshBuilder() 
	: startIndex(0)
	, drawMode(Renderer::DrawMode::TRIANGLES)
{

}

void MeshBuilder::Begin() {
	startIndex = vertices.size();
}

void MeshBuilder::End() {
	if (startIndex < vertices.size())
		startIndex = vertices.size();
}

void MeshBuilder::AddQuad(const Vec3f& bottomLeft, const Vec3f& up, float upLength, const Vec3f& right, float rightLength, const Vec3f& normal, const RGBA& color, const Vec2f& uvOffset, float uvStepSize) {
	unsigned int currentVert = vertices.size();

	SetColor(color);
	SetNormal(normal);

	SetUV(uvOffset + (Vec2f::UNIT_Y * uvStepSize));
	AddVertex(bottomLeft);		//020

	SetUV(uvOffset);
	AddVertex(bottomLeft + (up * upLength));//022

	SetUV(uvOffset + (Vec2f::ONE * uvStepSize));
	AddVertex(bottomLeft + (right * rightLength));//220

	SetUV(uvOffset + (Vec2f::UNIT_X * uvStepSize));
	AddVertex(bottomLeft + (up * upLength) + (right * rightLength));

	AddIndex(1 + currentVert);  
	AddIndex(2 + currentVert);	
	AddIndex(0 + currentVert);	
	AddIndex(3 + currentVert);	
	AddIndex(2 + currentVert);	
	AddIndex(1 + currentVert);	
}

void MeshBuilder::AddCube(float sideLength, const RGBA& color) {
	const float halfSideLength = sideLength / 2.0f;

	AddQuad(Vec3f::UP * sideLength, Vec3f::FORWARD, sideLength, Vec3f::RIGHT, sideLength, Vec3f::FORWARD * -1.0f, RGBA::CYAN, Vec2f::ZERO, 1.0f);										//TOP
	AddQuad(Vec3f::FORWARD * sideLength, Vec3f::FORWARD * -1.0f, sideLength, Vec3f::RIGHT, sideLength, Vec3f::FORWARD * -1.0f, RGBA::SADDLE_BROWN, Vec2f::ZERO, 1.0f);					//BOTTOM
	AddQuad(Vec3f::ZERO, Vec3f::UP, sideLength, Vec3f::RIGHT, sideLength, Vec3f::FORWARD * -1.0f, RGBA::RED, Vec2f::ZERO, 1.0f);														//SOUTH
	AddQuad(Vec3f::FORWARD * sideLength + Vec3f::RIGHT * sideLength, Vec3f::UP, sideLength, Vec3f::FORWARD * -1.0f, sideLength, Vec3f::FORWARD, RGBA::GREEN, Vec2f::ZERO, 1.0f);	//NORTH
	AddQuad(Vec3f::FORWARD * sideLength, Vec3f::UP, sideLength, Vec3f::FORWARD * -1.0f, sideLength, Vec3f::FORWARD * -1.0f, RGBA::VAPORWAVE, Vec2f::ZERO, 1.0f);						//WEST
	AddQuad(Vec3f::RIGHT * sideLength, Vec3f::UP, sideLength, Vec3f::FORWARD, sideLength, Vec3f::FORWARD * -1.0f, RGBA::YELLOW, Vec2f::ZERO, 1.0f);										//EAST
}

void MeshBuilder::AddVertex(const Vec3f& _position) {
	stamp.position = _position;
	vertices.emplace_back(stamp);
	SetMaskBit(POSITION_BIT);
}

void MeshBuilder::AddIndex(int index) {
	indices.emplace_back(index);
}

void MeshBuilder::CopyToMesh(const OpenGL& gl, Mesh* pMesh, VertexCopyCallback* copyFunction, unsigned int sizeofVertex) {
	unsigned int vertexCount = vertices.size();
	if (vertexCount == 0) {
		return;
	}

	unsigned int vertexSize = sizeofVertex;
	unsigned int vertexBufferSize = vertexCount * vertexSize;

	byte* vertexBuffer = new byte[vertexBufferSize];
	byte* currentBufferIndex = vertexBuffer;
	for (unsigned int vertexIndex = 0; vertexIndex < vertexCount; vertexIndex++) {
		copyFunction(vertices[vertexIndex], currentBufferIndex);
		currentBufferIndex += vertexSize;
	}
	//pMesh->Initialize(gl, &ColorVertex::BindVertexBuffer, vertexBuffer, vertexCount, sizeofVertex, indices.data(), indices.size());
	pMesh->Initialize(gl, &TexturedVertex::BindVertexBuffer, vertexBuffer, vertexCount, sizeofVertex, indices.data(), indices.size());
	pMesh->drawMode = this->drawMode;

	delete[] vertexBuffer;
	vertexBuffer = nullptr;
}

