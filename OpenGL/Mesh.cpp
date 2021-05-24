#include "Mesh.h"
#include "OpenGL.h"
#include "Renderer.h"
#include <string>

size_t Vertex::attributeCount = 1;

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

	attributeCount = 2;
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

	attributeCount = 3;
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
	:vertexCount(0), vertexArrayId(0), vertexBufferId(0), indexCount(0), indexBufferId(0)
{

}

Mesh::~Mesh() {

}

bool Mesh::Initialize(Renderer& renderer, VertexBufferBindCallback* pBindFuction, void* vertexData, unsigned int numVertices, unsigned int sizeofVertex, unsigned int* indexData, unsigned int numIndices) {
	vertexCount = numVertices;
	indexCount = numIndices;

	renderer.AllocateVertexBuffer(vertexArrayId, vertexBufferId, vertexData, pBindFuction, numVertices, sizeofVertex);
	renderer.AllocateIndexBuffer(indexBufferId, indexCount, indexData);

	return true;
}

void Mesh::Shutdown(Renderer& renderer) {
	renderer.DisableVertexAttribArray((size_t)TexturedVertex::attributeCount);

	renderer.ReleaseVertexBuffers(vertexBufferId, 1);
	renderer.ReleaseIndexBuffers(indexBufferId, 1);

	renderer.ReleaseVertexArray(vertexArrayId, 1);

}

void Mesh::Render(Renderer& renderer) {
	renderer.DrawIndexBuffer(vertexArrayId, indexCount);
}

///////////////////////////////// Mesh Builder /////////////////////////////
MeshBuilder::MeshBuilder() 
	: startIndex(0)
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

	AddQuad(Vec3f::UP * sideLength, Vec3f::FORWARD, sideLength, Vec3f::RIGHT, sideLength, Vec3f::UP, RGBA::CYAN, Vec2f::ZERO, 1.0f);										//TOP
	AddQuad(Vec3f::FORWARD * sideLength, Vec3f::FORWARD * -1.0f, sideLength, Vec3f::RIGHT, sideLength, Vec3f::UP * -1.0f, RGBA::SADDLE_BROWN, Vec2f::ZERO, 1.0f);					//BOTTOM
	AddQuad(Vec3f::ZERO, Vec3f::UP, sideLength, Vec3f::RIGHT, sideLength, Vec3f::FORWARD * -1.0f, RGBA::RED, Vec2f::ZERO, 1.0f);														//SOUTH
	AddQuad(Vec3f::FORWARD * sideLength + Vec3f::RIGHT * sideLength, Vec3f::UP, sideLength, Vec3f::FORWARD * -1.0f, sideLength, Vec3f::FORWARD, RGBA::GREEN, Vec2f::ZERO, 1.0f);	//NORTH
	AddQuad(Vec3f::FORWARD * sideLength, Vec3f::UP, sideLength, Vec3f::FORWARD * -1.0f, sideLength, Vec3f::RIGHT * -1.0f, RGBA::VAPORWAVE, Vec2f::ZERO, 1.0f);						//WEST
	AddQuad(Vec3f::RIGHT * sideLength, Vec3f::UP, sideLength, Vec3f::FORWARD, sideLength, Vec3f::RIGHT, RGBA::YELLOW, Vec2f::ZERO, 1.0f);										//EAST
}

void MeshBuilder::AddVertex(const Vec3f& _position) {
	stamp.position = _position;
	vertices.emplace_back(stamp);
	SetMaskBit(POSITION_BIT);
}

void MeshBuilder::AddIndex(int index) {
	indices.emplace_back(index);
}

void MeshBuilder::CopyToMesh(Renderer& renderer, Mesh* pMesh, VertexCopyCallback* copyFunction, unsigned int sizeofVertex) {
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
	pMesh->Initialize(renderer, &TexturedVertex::BindVertexBuffer, vertexBuffer, vertexCount, sizeofVertex, indices.data(), indices.size());

	delete[] vertexBuffer;
	vertexBuffer = nullptr;
}

