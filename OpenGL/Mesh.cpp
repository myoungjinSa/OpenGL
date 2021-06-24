#include "Mesh.h"
#include "OpenGL.h"
#include "Renderer.h"
#include "Logger.h"

#include <string>

//
//Vertex::Vertex()
//	: position()
//{
//}
//
//Vertex::Vertex(const Vec3f& pos) 
//	: position(pos)
//{
//}
//Vertex::~Vertex() {
//
//}


//ColorVertex::ColorVertex() 
//	: Vertex(), color()
//{
//
//}
//
//ColorVertex::ColorVertex(const Vec3f& pos, const RGBA& _color) 
//	: Vertex(pos), color(_color)
//{
//
//}
//
//
//ColorVertex::~ColorVertex() {
//
//}
//
//void ColorVertex::Copy(const VertexMaster& source, byte* pDestination) {
//	ColorVertex* pColorVertex = (ColorVertex*)(pDestination);
//	pColorVertex->position = source.position;
//	pColorVertex->color = source.color;
//
//}
//void ColorVertex::BindVertexBuffer(OpenGL& gl, void* pBuffer, unsigned int vertexBufferId, unsigned int vertexCount, unsigned int sizeofVertex) {
//	gl.glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
//	gl.glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(ColorVertex), pBuffer, GL_STATIC_DRAW);
//
//	gl.glEnableVertexAttribArray(0);		//Vertex Position
//	gl.glEnableVertexAttribArray(1);		//Vertex color.
//
//	gl.glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
//	gl.glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeofVertex, 0);
//
//	gl.glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
//	gl.glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, false, sizeofVertex, (unsigned char*)NULL + (3 * sizeof(float)));
//}

Vertex::Vertex() 
	:position(), uv0(), normal()
{

}
Vertex::Vertex(const Vec3f& pos, const Vec2f& _uv0, const Vec3f& _normal) 
	: position(pos), uv0(_uv0), normal(_normal)
{

}

Vertex::~Vertex() {

}
void Vertex::Copy(const VertexMaster& source, byte* pDestination) {
	Vertex* pVertex = (Vertex*)(pDestination);
	pVertex->position = source.position;
	pVertex->uv0 = source.uv0;
	pVertex->normal = source.normal;
}

void Vertex::BindVertexBuffer(OpenGL& gl, void* pBuffer, unsigned int vertexBufferId, unsigned int vertexCount, unsigned int sizeofVertex) {
	gl.glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	gl.glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(Vertex), pBuffer, GL_STATIC_DRAW);

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

//함수 외부에서 free를 해줘야함
byte* Vertex::ReadBufferData(byte* pBuffer, size_t targetDataSize) {
	if (!pBuffer)
		return nullptr;

	byte* pReturn = (byte*)malloc(targetDataSize * sizeof(byte));
	if (!pReturn)
		return nullptr;

	memcpy_s(pReturn, targetDataSize * sizeof(byte), pBuffer, targetDataSize * sizeof(byte));
	return pReturn;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
Triangle::Triangle() {
	
}

Triangle::~Triangle() {

}

void Triangle::Set(const Vertex& v0, const Vertex& v1, const Vertex& v2) {
	vertices[0] = v0;
	vertices[1] = v1;
	vertices[2] = v2;
}

Vertex* Triangle::Get(size_t index) {
	if (3 <= index) {
		LogError(L"Error-Triangles::Get() - The index is out of range.\n");
		return nullptr;
	}
	return &vertices[index];
}


void Triangle::SetPosition(size_t index, float posX, float posY, float posZ) {
	if (Vertex* pVertex = Get(index)) {
		pVertex->position.x = posX;
		pVertex->position.y = posY;
		pVertex->position.z = posZ;
	}
}

void Triangle::SetPosition(size_t index, const Vec3f& pos) {
	if (Vertex* pVertex = Get(index)) {
		pVertex->position = pos;
	}
}

Vec3f Triangle::GetPosition(size_t index) const {
	if (3 <= index) {
		LogError(L"Error-Triangles::GetPosition() - The index is out of range.\n");
		return Vec3f();
	}

	return vertices[index].position;
}


void Triangle::SetNormal(size_t index, float normalX, float normalY, float normalZ) {
	if (Vertex* pVertex = Get(index)) {
		pVertex->normal.x = normalX;
		pVertex->normal.y = normalY;
		pVertex->normal.z = normalZ;
	}
}

void Triangle::SetNormal(size_t index, const Vec3f& normal) {
	if (Vertex* pVertex = Get(index)) {
		pVertex->normal.x = normal.x;
		pVertex->normal.y = normal.y;
		pVertex->normal.z = normal.z;
	}
}

Vec3f Triangle::GetNormal(size_t index) const {
	if (3 <= index) {
		LogError(L"Error-Triangles::GetNormal() - The index is out of range.\n");
		return Vec3f();
	}

	return vertices[index].normal;
}



void Triangle::SetUV(size_t index, float u, float v) {
	if (Vertex* pVertex = Get(index)) {
		pVertex->uv0.x = u;
		pVertex->uv0.y = v;
	}
}

void Triangle::SetUV(size_t index, const Vec2f& uv) {
	if (Vertex* pVertex = Get(index)) {
		pVertex->uv0.x = uv.x;
		pVertex->uv0.y = uv.y;
	}
}


Vec2f Triangle::GetUV(size_t index) const {
	if (2 <= index) {
		LogError(L"Error-Triangles::GetUV() - The index is out of range.\n");
		return Vec2f();
	}

	return vertices[index].uv0;
}


Mesh::Mesh()
	:tri(), vertexCount(0), vertexArrayId(0), vertexBufferId(0), indexCount(0), indexBufferId(0)
{

}

Mesh::~Mesh() {

}

bool Mesh::Initialize(Renderer& renderer, VertexBufferBindCallback* pBindFuction, void* vertexData, unsigned int numVertices, unsigned int sizeofVertex, unsigned int* indexData, unsigned int numIndices) {
	vertexCount = numVertices;
	indexCount = numIndices;

	Vec3f* pPosBuffer = reinterpret_cast<Vec3f*>(Vertex::ReadBufferData(static_cast<byte*>(vertexData), sizeof(Vec3f)));
	if (!pPosBuffer)
		return false;
	
	LogDebug(L"Vertex Position : %5lf, %5lf, %5lf\n", pPosBuffer->x, pPosBuffer->y, pPosBuffer->z);;

	free(pPosBuffer); pPosBuffer = nullptr;

	Vec2f* pUVBuffer = reinterpret_cast<Vec2f*>(Vertex::ReadBufferData(static_cast<byte*>(vertexData), sizeof(Vec3f) + sizeof(Vec2f)));
	if (!pUVBuffer)
		return false;

	LogDebug(L"UV Position : %5lf, %5lf\n", pUVBuffer->x, pUVBuffer->y);;
	
	free(pUVBuffer); pUVBuffer = nullptr;

	Vec3f* pNormalBuffer = reinterpret_cast<Vec3f*>(Vertex::ReadBufferData(static_cast<byte*>(vertexData), sizeof(Vec3f) + sizeof(Vec2f) + sizeof(Vec3f)));
	if (!pNormalBuffer)
		return false;

	LogDebug(L"UV Position : %5lf, %5lf\n", pNormalBuffer->x, pNormalBuffer->y);;
	free(pNormalBuffer); pNormalBuffer = nullptr;

	renderer.AllocateVertexBuffer(vertexArrayId, vertexBufferId, vertexData, pBindFuction, numVertices, sizeofVertex);
	renderer.AllocateIndexBuffer(indexBufferId, indexCount, indexData);

	
	return true;
}

void Mesh::Shutdown(Renderer& renderer) {
	renderer.DisableVertexAttribArray(3);

	renderer.ReleaseVertexBuffers(vertexBufferId, 1);
	renderer.ReleaseIndexBuffers(indexBufferId, 1);

	renderer.ReleaseVertexArray(vertexArrayId, 1);

}

void Mesh::Render(Renderer& renderer) {
	//renderer.DrawVertexBuffer(vertexArrayId, 0, vertexCount);
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
	pMesh->Initialize(renderer, &Vertex::BindVertexBuffer, vertexBuffer, vertexCount, sizeofVertex, indices.data(), indices.size());

	delete[] vertexBuffer;
	vertexBuffer = nullptr;
}

