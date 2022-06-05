#include "Mesh.h"
#include "OpenGL.h"
#include "Renderer.h"
#include "Logger.h"
#include "Transform.h"
#include "Terrain.h"

#include <tuple>
#include <string>


//함수 외부에서 free를 해줘야함
byte* ReadBufferData(void* pBuffer, size_t targetDataSize) {
	if (!pBuffer)
		return nullptr;

	byte* pReturn = (byte*)malloc(targetDataSize * sizeof(byte));
	if (!pReturn)
		return nullptr;

	memset(pReturn, 0, targetDataSize * sizeof(byte));
	memcpy_s(pReturn, targetDataSize * sizeof(byte), pBuffer, targetDataSize * sizeof(byte));
	return pReturn;
}


Vertex::Vertex() 
	:position(), uv0(), normal(), color()
{

}
Vertex::Vertex(const Vec3f& pos, const Vec2f& _uv0, const Vec3f& _normal, const RGBA& _color) 
	: position(pos), uv0(_uv0), normal(_normal), color(_color)
{

}

void Vertex::Copy(const VertexMaster& source, byte* pDestination) {
	Vertex* pVertex = (Vertex*)(pDestination);
	pVertex->position = source.position;
	pVertex->color = source.color;
	pVertex->uv0 = source.uv0;
	pVertex->normal = source.normal;
}

void Vertex::BindVertexBuffer(OpenGL& gl, void* pBuffer, unsigned int vertexBufferId, unsigned int vertexCount, unsigned int sizeofVertex) {
	gl.glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	gl.glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(Vertex), pBuffer, GL_STATIC_DRAW);

	gl.glEnableVertexAttribArray(0); // Vertex Position;
	gl.glEnableVertexAttribArray(1); // Color
	gl.glEnableVertexAttribArray(2); // Texture coordinates;
	gl.glEnableVertexAttribArray(3); // Normal
	gl.glEnableVertexAttribArray(4); // Tangent

	gl.glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	gl.glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeofVertex, 0);

	gl.glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	gl.glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, false, sizeofVertex, (unsigned char*)NULL + (3 * sizeof(float)));

	gl.glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	gl.glVertexAttribPointer(2, 2, GL_FLOAT, false, sizeofVertex, (unsigned char*)NULL + (3 * sizeof(float)) + (4 * sizeof(unsigned char)));

	gl.glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	gl.glVertexAttribPointer(3, 3, GL_FLOAT, false, sizeofVertex, (unsigned char*)NULL + (5 * sizeof(float)) + (4 * sizeof(unsigned char)));

	gl.glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	gl.glVertexAttribPointer(4, 3, GL_FLOAT, false, sizeofVertex, (unsigned char*)NULL + (8 * sizeof(float)) + (4 * sizeof(unsigned char)));
}


//////////////////////////////////////////////////////////////////////////////////////////////////
Triangle::Triangle()
{
	
}


Triangle::Triangle(const Vertex& v0, const Vertex& v1, const Vertex& v2)
{
	Set(v0, v1, v2);
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
	:triangles(), vertexList(), vertexCount(0), vertexArrayId(0), vertexBufferId(0), indexCount(0), indexBufferId(0)
{

}

Mesh::~Mesh() {
	for (uint32_t iVert = 0; iVert < vertexList.size(); ++iVert) {
		delete vertexList[iVert];
		vertexList[iVert] = nullptr;
	}
	vertexList.clear();
}

bool Mesh::Initialize(Renderer& renderer, VertexBufferBindCallback* pBindFunction, void* vertexData, unsigned int numVertices, unsigned int sizeofVertex, unsigned int* indexData, unsigned int numIndices) {
	vertexCount = numVertices;
	indexCount = numIndices;

	renderer.AllocateVertexBuffer(vertexArrayId, vertexBufferId, vertexData, pBindFunction, numVertices, sizeofVertex);
	renderer.AllocateIndexBuffer(indexBufferId, indexCount, indexData);

	BuildVertexList(vertexData);
	BuildIndexList(indexData);

	return true;
}

void Mesh::Shutdown(Renderer& renderer) {
	renderer.DisableVertexAttribArray(3);

	renderer.ReleaseVertexBuffers(vertexBufferId, 1);
	renderer.ReleaseIndexBuffers(indexBufferId, 1);

	renderer.ReleaseVertexArray(vertexArrayId, 1);

}

void Mesh::Render(Renderer& renderer) {
	if (DoesHaveIndexBuffer()) {
		renderer.DrawIndexBuffer(vertexArrayId, indexCount);
	}else {
		renderer.DrawVertexBuffer(vertexArrayId, 0, vertexCount);
	}
}


bool Mesh::BuildTriangles(TriangleType triangleType) {
	if (triangleType == TriangleType::MeshType_List) {
		if (DoesHaveIndexBuffer()) {
			for (size_t iIndex = 0; iIndex < indexCount - 2; iIndex += 3) {
				std::tuple<const Vertex*, const Vertex*, const Vertex*> vTuple = std::make_tuple(vertexList.at(indexList[iIndex]), vertexList.at(indexList[iIndex + 1]), vertexList.at(indexList[iIndex + 2]));
				triangles.push_back(Triangle(*std::get<0>(vTuple), *std::get<1>(vTuple), *std::get<2>(vTuple)));
			}
		}
		else {
			for (size_t iVertex = 0; iVertex < vertexCount - 2; iVertex += 3) {
				std::tuple<const Vertex*, const Vertex*, const Vertex*> vTuple = std::make_tuple(vertexList.at(iVertex), vertexList.at(iVertex + 1), vertexList.at(iVertex + 2));
				triangles.push_back(Triangle(*std::get<0>(vTuple), *std::get<1>(vTuple), *std::get<2>(vTuple)));
			}
		}
	}
	else if (triangleType == TriangleType::MeshType_Strip) {
		if (DoesHaveIndexBuffer()) {
			for (size_t iIndex = 0; iIndex < indexCount - 2; iIndex++) {
				std::tuple<const Vertex*, const Vertex*, const Vertex*> vTuple = std::make_tuple(vertexList.at(indexList[iIndex]), vertexList.at(indexList[iIndex + 1]), vertexList.at(indexList[iIndex + 2]));
				triangles.push_back(Triangle(*std::get<0>(vTuple), *std::get<1>(vTuple), *std::get<2>(vTuple)));
			}
		}
		else {
			//Not yet...
			assert(0);
		}
	}
	return true;
}


bool Mesh::BuildVertexList(void* vertexDatas) {
	size_t offset = 0;
	for (size_t iVertex = 0; iVertex < vertexCount; iVertex++) {
		Vec3f* pPosBuffer = reinterpret_cast<Vec3f*>(ReadBufferData(static_cast<byte*>(vertexDatas) + offset, sizeof(Vec3f)));
		if (!pPosBuffer)
			return false;

		offset += sizeof(Vec3f);
		//LogDebug(L"Vertex Position : %5lf, %5lf, %5lf\n", pPosBuffer->x, pPosBuffer->y, pPosBuffer->z);


		RGBA* pColorBuffer = reinterpret_cast<RGBA*>(ReadBufferData(static_cast<byte*>(vertexDatas) + offset, sizeof(RGBA)));
		if (!pColorBuffer)
			return false;

		offset += sizeof(RGBA);
		//LogDebug(L"Color : %5lf, %5lf, %5lf, %5lf\n", pColorBuffer->red, pColorBuffer->green, pColorBuffer->blue, pColorBuffer->alpha);


		Vec2f* pUVBuffer = reinterpret_cast<Vec2f*>(ReadBufferData(static_cast<byte*>(vertexDatas) + offset, sizeof(Vec2f)));
		if (!pUVBuffer)
			return false;

		offset += sizeof(Vec2f);
		//LogDebug(L"UV  : %5lf, %5lf\n", pUVBuffer->x, pUVBuffer->y);


		Vec3f* pNormalBuffer = reinterpret_cast<Vec3f*>(ReadBufferData(static_cast<byte*>(vertexDatas) + offset, sizeof(Vec3f)));
		if (!pNormalBuffer)
			return false;

		offset += sizeof(Vec3f);
		//LogDebug(L"Normal : %5lf, %5lf, %5lf\n", pNormalBuffer->x, pNormalBuffer->y, pNormalBuffer->z);


		vertexList.push_back(new Vertex(*pPosBuffer, *pUVBuffer, *pNormalBuffer, *pColorBuffer));

		free(pPosBuffer); pPosBuffer = nullptr;
		free(pColorBuffer); pColorBuffer = nullptr;
		free(pUVBuffer); pUVBuffer = nullptr;
		free(pNormalBuffer); pNormalBuffer = nullptr;
	}
	return true;
}

bool Mesh::BuildIndexList(unsigned int* indicesData) {
	for (size_t iIndices = 0; iIndices < indexCount; iIndices++) {
		indexList.push_back(indicesData[iIndices]);
	}
	return true;
}

size_t Mesh::GetTriangleMeshCount() const {
	return triangles.size();
}
const Triangle& Mesh::GetTriangleMesh(size_t index) const {
	if (triangles.size() <= index) {
		LogError(L"Triangle::GetTriangleMesh() - Index is out of range");
		assert(0);
		return *triangles.end();
	}
	return triangles.at(index);
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

	vertices.clear();
	indices.clear();
}

//Pivot이 왼쪽 아래
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

//Pivot이 중앙
void MeshBuilder::AddQuad(const Vec3f& center, const Vec3f& extent, const Vec3f& _normal, const RGBA& color, const Vec2f& uvOffset, float uvStepSize, bool bReversed) {
	unsigned int currentVert = vertices.size();
	SetColor(color);
	Vec3f normal = bReversed ? _normal * -1.0f : _normal;
	SetNormal(normal);

	//Left Top
	SetUV(bReversed ? Vec2f(1.0f, 1.0f) : Vec2f(0.0f, 1.0f));
	AddVertex(Vec3f(center.x - extent.x, center.y + extent.y, center.z - extent.z));

	//Right Top
	SetUV(bReversed ? Vec2f(0.0f, 1.0f) : Vec2f(1.0f, 1.0f));
	AddVertex(Vec3f(center.x + extent.x, center.y + extent.y, center.z + extent.z));

	//LeftBottom
	SetUV(bReversed ? Vec2f(1.0f, 0.0f) : Vec2f(0.0f, 0.0f));
	AddVertex(Vec3f(center.x - extent.x, center.y - extent.y, center.z - extent.z));

	//Right Bottom
	SetUV(bReversed ? Vec2f(0.0f, 0.0f) : Vec2f(1.0f, 0.0f));
	AddVertex(Vec3f(center.x + extent.x, center.y - extent.y, center.z + extent.z));

	if (bReversed) {
		AddIndex(1 + currentVert);
		AddIndex(0 + currentVert);
		AddIndex(2 + currentVert);
		AddIndex(1 + currentVert);
		AddIndex(2 + currentVert);
		AddIndex(3 + currentVert);
	}
	else {
		AddIndex(0 + currentVert);
		AddIndex(3 + currentVert);
		AddIndex(2 + currentVert);
		AddIndex(0 + currentVert);
		AddIndex(1 + currentVert);
		AddIndex(3 + currentVert);
	}
}

void MeshBuilder::AddCylinder(const Vec3f& axis, const Vec3f& arm1, const Vec3f& arm2, uint32_t sliceCount, const RGBA& color) {

	SetColor(color);
	for (uint32_t iSlice = 0; iSlice <= sliceCount; ++iSlice) {
		const float tex_s = static_cast<float>(iSlice) / sliceCount, angle = (float)(iSlice % sliceCount) * MathUtils::TWO_PI / sliceCount;
		const Vec3f arm = arm1 * std::cos(angle) + arm2 * std::sin(angle);
		
		SetPosition(arm);
		SetNormal(Normalize(arm));

		vertices.emplace_back(stamp);
		
		SetColor(color);
		SetPosition(arm + axis);
		SetNormal(Normalize(arm));

		vertices.emplace_back(stamp);
	}
	for (uint32_t iSlice = 0; iSlice < sliceCount; ++iSlice) {
		AddIndex(iSlice * 2);
		AddIndex(iSlice * 2 + 3);
		AddIndex(iSlice * 2 + 2);

		AddIndex(iSlice * 2);
		AddIndex(iSlice * 2 + 1);
		AddIndex(iSlice * 2 + 3);
	}

	//Generate caps
	uint32_t base = (uint32_t)vertices.size();
	for (uint32_t iSlice = 0; iSlice < sliceCount; ++iSlice) {
		const float angle = static_cast<float>(iSlice % sliceCount) * MathUtils::TWO_PI / sliceCount;
		const float cosAngle = std::cos(angle);
		const float sinAngle = std::sin(angle);
		
		const Vec3f arm = arm1 * cosAngle + arm2 * sinAngle;
		SetColor(color); 
		SetPosition(arm + axis);
		SetNormal(Normalize(axis));

		vertices.push_back(stamp);

		SetColor(color);
		SetPosition(arm);
		SetNormal(-Normalize(axis));

		vertices.push_back(stamp);
	}

	for (uint32_t iSlice = 2; iSlice < sliceCount; ++iSlice) {
		AddIndex(base);
		AddIndex(base + iSlice * 2);
		AddIndex(base + iSlice * 2 - 2);

		AddIndex(base + 1);
		AddIndex(base + iSlice * 2 - 1);
		AddIndex(base + iSlice * 2 + 1);
	}
}

void MeshBuilder::AddXAxisCircle(float outerRadius, float innerRadius, const RGBA& color) {
	const int segmentCount = 32;

	for (int iSegment = 0; iSegment < segmentCount + 1; iSegment++) {
		float angle = MathUtils::PI * 2 * (float)iSegment / (float)segmentCount;
		const float xOffset = 0.03f;
		
		SetPosition(Vec3f(xOffset, cosf(angle) * outerRadius, sinf(angle) * outerRadius));
		SetColor(color);
		vertices.push_back(stamp);

		SetPosition(Vec3f(-xOffset, cosf(angle) * innerRadius, sinf(angle) * innerRadius));
		SetColor(color);
		vertices.push_back(stamp);
	}

	for (size_t iVertex = 0; iVertex < vertices.size(); iVertex++) {
		AddIndex(iVertex);
	}
}

void MeshBuilder::AddYAxisCircle(float outerRadius, float innerRadius, const RGBA& color) {
	const int segmentCount = 32;

	for (int iSegment = 0; iSegment < segmentCount + 1; iSegment++) {
		float angle = MathUtils::PI * 2 * (float)iSegment / (float)segmentCount;
		const float yOffset = 0.03f;
		SetPosition(Vec3f(sinf(angle) * outerRadius, yOffset, cosf(angle) * outerRadius));
		SetColor(color);
		vertices.push_back(stamp);

		SetPosition(Vec3f(sinf(angle) * innerRadius, -yOffset, cosf(angle) * innerRadius));
		SetColor(color);
		vertices.push_back(stamp);
	}

	for (size_t iVertex = 0; iVertex < vertices.size() - 2; iVertex++) {
		//for triangle strip
		AddIndex(iVertex);
	}
}

void MeshBuilder::AddZAxisCircle(float outerRadius, float innerRadius, const RGBA& color) {
	const int segmentCount = 32;
	
	for (int iSegment = 0; iSegment < segmentCount + 1; iSegment++) {
		float angle = MathUtils::PI * 2 * (float)iSegment / (float)segmentCount;
		float zOffset = 0.03f;
		SetPosition(Vec3f(cosf(angle) * outerRadius, sinf(angle) * outerRadius, zOffset));
		SetColor(color);
		vertices.push_back(stamp);

		SetPosition(Vec3f(cosf(angle) * innerRadius, sinf(angle) * innerRadius, -zOffset));
		SetColor(color);
		vertices.push_back(stamp);
	}

	for (size_t iVertex = 0; iVertex < vertices.size(); iVertex++) {
		//for triangle strip
		AddIndex(iVertex);
	}

}

void MeshBuilder::AddXAxisCone(const Vec3f& centerOffset, float halfWidth, float halfHeight, float halfDepth, double angleStep, const RGBA& color) {

	float fReciprocalPrecisition = (float)(angleStep / MathUtils::TWO_PI);

	int iSectorCount = 0;

	for (double coverAngle1 = 0.0f; coverAngle1 < MathUtils::TWO_PI; coverAngle1 += angleStep) {
		double coverAngle2 = coverAngle1 + angleStep;
		float heightPart1 = (float)(halfHeight * cos(coverAngle1));
		float heightPart2 = (float)(halfHeight * cos(coverAngle2));
		float depthPart1 = (float)(halfDepth * sin(coverAngle1));
		float depthPart2 = (float)(halfDepth * sin(coverAngle2));

		float fTextureOffsetS1 = iSectorCount * fReciprocalPrecisition;
		float fTextureOffsetS2 = (iSectorCount + 1) * fReciprocalPrecisition;

		SetColor(color);
		SetPosition(centerOffset + Vec3f(halfWidth + halfWidth, 0.0f, 0.0f));
		SetUV(fTextureOffsetS1, 1.0f);
		SetNormal(Vec3f(0.0f, (heightPart1 + heightPart2) / 2.0f, (depthPart1 + depthPart2) / 2.0f));
		vertices.push_back(stamp);

		SetColor(color);
		SetPosition(centerOffset + Vec3f(0.0f, heightPart2, depthPart2));
		SetUV(fTextureOffsetS2, 0.0f);
		SetNormal(Vec3f(0.0f, (heightPart1 + heightPart2) / 2.0f, (depthPart1 + depthPart2) / 2.0f));
		vertices.push_back(stamp);

		SetColor(color);
		SetPosition(centerOffset + Vec3f(0.0f, heightPart1, depthPart1));
		SetUV(fTextureOffsetS2, 0.0f);
		SetNormal(Vec3f(0.0f, (heightPart1 + heightPart2) / 2.0f, (depthPart1 + depthPart2) / 2.0f));
		vertices.push_back(stamp);

		iSectorCount++;
	}

	//Cone Bottom
	for (double bottomAngle = 0; bottomAngle < MathUtils::TWO_PI; bottomAngle += angleStep) {
		SetColor(color);
		SetNormal(Vec3f(-1.0f, 0.0f, 0.0f));

		double c = cos(bottomAngle);
		double s = sin(bottomAngle);

		float fTextureOffsetS = 0.5f + (float)(0.5f * c);
		float fTextureOffsetT = 0.5f + (float)(0.5f * s);

		SetUV(fTextureOffsetS, fTextureOffsetT);
		SetPosition(centerOffset + Vec3f(0.0f, (float)(halfHeight * c), (float)(halfDepth * s)));
		vertices.push_back(stamp);
	}
}
void MeshBuilder::AddYAxisCone(const Vec3f& centerOffset, float halfWidth, float halfHeight, float halfDepth, double angleStep, const RGBA& color) {
	float fReciprocalPrecisition = (float)(angleStep / MathUtils::TWO_PI);

	int iSectorCount = 0;
	
	for (double coverAngle1 = 0.0f; coverAngle1 < MathUtils::TWO_PI; coverAngle1 += angleStep) {
		double coverAngle2 = coverAngle1 + angleStep;
		float widthPart1 = (float)(halfWidth * cos(coverAngle1));
		float widthPart2 = (float)(halfWidth * cos(coverAngle2));
		float depthPart1 = (float)(halfDepth * sin(coverAngle1));
		float depthPart2 = (float)(halfDepth * sin(coverAngle2));

		float fTextureOffsetS1 = iSectorCount * fReciprocalPrecisition;
		float fTextureOffsetS2 = (iSectorCount + 1) * fReciprocalPrecisition;

		SetColor(color);
		SetPosition(centerOffset + Vec3f(0.0f, halfHeight + halfHeight, 0.0f));
		SetUV(fTextureOffsetS1, 1.0f);
		SetNormal(Vec3f((widthPart1 + widthPart2) / 2.0f, 0.0f, (depthPart1 + depthPart2) / 2.0f));
		vertices.push_back(stamp);

		SetColor(color);
		SetPosition(centerOffset + Vec3f(widthPart2, 0.0f, depthPart2));
		SetUV(fTextureOffsetS2, 0.0f);
		SetNormal(Vec3f((widthPart1 + widthPart2) / 2.0f, 0.0f, (depthPart1 + depthPart2) / 2.0f));
		vertices.push_back(stamp);

		SetColor(color);
		SetPosition(centerOffset + Vec3f(widthPart1 ,0.0f, depthPart1));
		SetUV(fTextureOffsetS2, 0.0f);
		SetNormal(Vec3f((widthPart1 + widthPart2) / 2.0f, 0.0f, (depthPart1 + depthPart2) / 2.0f));
		vertices.push_back(stamp);

		iSectorCount++;
	}

	//Cone Bottom
	for (double bottomAngle = 0; bottomAngle < MathUtils::TWO_PI; bottomAngle += angleStep) {
		SetColor(color);
		SetNormal(Vec3f(0.0f, -1.0f, 0.0f));

		double c = cos(bottomAngle);
		double s = sin(bottomAngle);

		float fTextureOffsetS = 0.5f + (float)(0.5f * c);
		float fTextureOffsetT = 0.5f + (float)(0.5f * s);

		SetUV(fTextureOffsetS, fTextureOffsetT);
		SetPosition(centerOffset + Vec3f((float)(halfWidth * c), 0.0f, (float)(halfDepth * s)));
		vertices.push_back(stamp);
	}
}

void MeshBuilder::AddZAxisCone(const Vec3f& centerOffset, float halfWidth, float halfHeight, float halfDepth, double angleStep, const RGBA& color) {
	float fReciprocalPrecisition = (float)(angleStep / MathUtils::TWO_PI);

	int iSectorCount = 0;

	for (double coverAngle1 = 0.0f; coverAngle1 < MathUtils::TWO_PI; coverAngle1 += angleStep) {
		double coverAngle2 = coverAngle1 + angleStep;
		float heightPart1 = (float)(halfHeight * cos(coverAngle1));
		float heightPart2 = (float)(halfHeight * cos(coverAngle2));
		float widthPart1 = (float)(halfWidth * sin(coverAngle1));
		float widthPart2 = (float)(halfWidth * sin(coverAngle2));

		float fTextureOffsetS1 = iSectorCount * fReciprocalPrecisition;
		float fTextureOffsetS2 = (iSectorCount + 1) * fReciprocalPrecisition;

		SetColor(color);
		SetPosition(centerOffset + Vec3f(0.0f, 0.0f, halfDepth + halfDepth));
		SetUV(fTextureOffsetS1, 1.0f);
		SetNormal(Vec3f((widthPart1 + widthPart2) / 2.0f, (heightPart1 + heightPart2) / 2.0f, 0.0f));
		vertices.push_back(stamp);

		SetColor(color);
		SetPosition(centerOffset + Vec3f(widthPart2, heightPart2, 0.0f));
		SetUV(fTextureOffsetS2, 0.0f);
		SetNormal(Vec3f((widthPart1 + widthPart2) / 2.0f, (heightPart1 + heightPart2) / 2.0f, 0.0f));
		vertices.push_back(stamp);

		SetColor(color);
		SetPosition(centerOffset + Vec3f(widthPart1, heightPart1, 0.0f));
		SetUV(fTextureOffsetS2, 0.0f);
		SetNormal(Vec3f((widthPart1 + widthPart2) / 2.0f, (heightPart1 + heightPart2) / 2.0, 0.0f));
		vertices.push_back(stamp);

		iSectorCount++;
	}

	//Cone Bottom
	for (double bottomAngle = 0; bottomAngle < MathUtils::TWO_PI; bottomAngle += angleStep) {
		SetColor(color);
		SetNormal(Vec3f(0.0f, 0.0f, -1.0f));

		double c = cos(bottomAngle);
		double s = sin(bottomAngle);

		float fTextureOffsetS = 0.5f + (float)(0.5f * c);
		float fTextureOffsetT = 0.5f + (float)(0.5f * s);

		SetUV(fTextureOffsetS, fTextureOffsetT);
		SetPosition(centerOffset + Vec3f((float)(halfWidth * c), (float)(halfHeight * s), 0.0f));
		vertices.push_back(stamp);
	}
}


void MeshBuilder::AddSphere(const Vec3f& center, float radius, int sectorCount, int stackCount) {
	const float PI = acos(-1);

	float nx, ny, nz, lengthInv = 1.0f / radius;

	// build vertices of sphere with smooth shading using parametric equation
	// x = r * cos(u) * cos(v)
	// y = r * cos(u) * sin(v)
	// z = r * sin(u)
	//u: stack(latitude) angle(-90 <= u <= 90)
	//v: sector(longitude) angle(0 <= v <= 360)
	
	float sectorStep = 2 * PI / sectorCount;				// starting from pi/2 to -pi/2
	float stackStep = PI / stackCount;
	float sectorAngle, stackAngle;

	for (int iStack = 0; iStack <= stackCount; iStack++) {
		stackAngle = PI / 2 - iStack * stackStep;
		float yz = radius * cosf(stackAngle);
		float x = radius * sinf(stackAngle);
		float y = 0.0f, z = 0.0f;

		// add (sectorCount+1) vertices per stack
		// the first and last vertices have same position and normal, but different tex coords
		for (int iSector = 0; iSector <= sectorCount; iSector++) {
			sectorAngle = iSector * sectorStep;
			//Vertex position
			y = yz * cosf(sectorAngle);
			z = yz * sinf(sectorAngle);
			SetPosition(x + center.x, y + center.y, z + center.z);

			//normalized vertex normal
			Vec3f n = Normalize(Vec3f(x, y, z) - center);
			SetNormal(n);

			// vertex tex coord between [0, 1]
			float s = (float)iSector / sectorCount;
			float t = (float)iStack / stackCount;
			SetUV(s, t);
			

			vertices.emplace_back(stamp);
		}
	}

	// indices
	//  k1------k1+1
	//  |	 /	|
	//  |  /	|
	//  k2----- k2+1

	unsigned int k1, k2, k3, k4;

	for (int iStack = 0; iStack < stackCount; iStack++) {
		k1 = iStack * (sectorCount + 1);     // beginning of current stack
		k2 = k1 + sectorCount + 1;
		for (int iSector = 0; iSector < sectorCount; iSector++, k1++, k2++) {
			if (iStack != 0) {
				AddIndex(k1); AddIndex(k2); AddIndex(k1 + 1);
			}

			if (iStack != (stackCount - 1)) {
				AddIndex(k2), AddIndex(k2 + 1); AddIndex(k1 + 1);
			}
		}
	}
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

void MeshBuilder::AddCube(const Vec3f& center, const Vec3f& extent, const RGBA& color, bool bLeftHand) {
	
	//front
	Vec3f leftTopFront = Vec3f(center.x - extent.x, center.y + extent.y, center.z - extent.z);
	Vec3f rightTopFront = Vec3f(center.x + extent.x, center.y + extent.y, center.z - extent.z);
	Vec3f leftBottomFront = Vec3f(center.x - extent.x, center.y - extent.y, center.z - extent.z);
	Vec3f rightBottomFront = Vec3f(center.x + extent.x, center.y - extent.y, center.z - extent.z);


	//Back
	Vec3f leftTopBack = Vec3f(center.x - extent.x, center.y + extent.y, center.z + extent.z);
	Vec3f rightTopBack = Vec3f(center.x + extent.x, center.y + extent.y, center.z + extent.z);
	Vec3f leftBottomBack = Vec3f(center.x - extent.x, center.y - extent.y, center.z + extent.z);
	Vec3f rightBottomBack = Vec3f(center.x + extent.x, center.y - extent.y, center.z + extent.z);

	SetColor(color);
	
	//Front
	//Left Top Front
	SetUV(Vec2f(0.0f, 1.0f));
	Vec3f sumNormals = ((Vec3f::FORWARD * -1) + (Vec3f::RIGHT * -1) + (Vec3f::UP)) / 3;
	sumNormals.Normalize();

	SetNormal(sumNormals);
	AddVertex(leftTopFront);

	//RightTop Front
	SetUV(Vec2f(1.0f, 1.0f));

	sumNormals = ((Vec3f::FORWARD * -1) + (Vec3f::RIGHT) + (Vec3f::UP)) / 3;
	sumNormals.Normalize();
	SetNormal(sumNormals);
	AddVertex(rightTopFront);

	//left Bottom Front
	SetUV(Vec2f(0.0f, 0.0f));

	sumNormals = ((Vec3f::FORWARD * -1) + (Vec3f::RIGHT * -1) + (Vec3f::UP * -1)) / 3;
	sumNormals.Normalize();
	SetNormal(sumNormals);
	AddVertex(leftBottomFront);

	//right Bottom Front
	SetUV(Vec2f(1.0f, 0.0f));

	sumNormals = ((Vec3f::FORWARD * -1) + (Vec3f::RIGHT) + (Vec3f::UP * -1)) / 3;
	sumNormals.Normalize();
	SetNormal(sumNormals);
	AddVertex(rightBottomFront);


	//Left Top Back
	SetUV(Vec2f(1.0f, 1.0f));

	sumNormals = ((Vec3f::FORWARD) + (Vec3f::RIGHT * -1.0f) + (Vec3f::UP)) / 3;
	sumNormals.Normalize();
	SetNormal(sumNormals);
	AddVertex(leftTopBack);

	//Right Top Back
	SetUV(Vec2f(0.0f, 1.0f));

	sumNormals = ((Vec3f::FORWARD) + (Vec3f::RIGHT) + (Vec3f::UP)) / 3;
	sumNormals.Normalize();
	SetNormal(sumNormals);
	AddVertex(rightTopBack);

	//left Bottom Back
	SetUV(Vec2f(1.0f, 0.0f));

	sumNormals = ((Vec3f::FORWARD) + (Vec3f::RIGHT * -1.0f) + (Vec3f::UP * -1.0f)) / 3;
	sumNormals.Normalize();
	SetNormal(sumNormals);
	AddVertex(leftBottomBack);

	//right Bottom Back
	SetUV(Vec2f(0.0f, 0.0f));

	sumNormals = ((Vec3f::FORWARD) + (Vec3f::RIGHT) + (Vec3f::UP * -1.0f)) / 3;
	sumNormals.Normalize();
	SetNormal(sumNormals);
	AddVertex(rightBottomBack);
	
	
	if (bLeftHand) {
		//Front
		AddIndex(0); AddIndex(1); AddIndex(2);
		AddIndex(1); AddIndex(3); AddIndex(2);

		//Left
		AddIndex(4); AddIndex(0); AddIndex(6);
		AddIndex(0); AddIndex(2); AddIndex(6);

		//Right
		AddIndex(5); AddIndex(7); AddIndex(3);
		AddIndex(5); AddIndex(3); AddIndex(1);

		//Back
		AddIndex(6); AddIndex(5); AddIndex(4);
		AddIndex(6); AddIndex(7); AddIndex(5);

		//Top
		AddIndex(0); AddIndex(4); AddIndex(1);
		AddIndex(4); AddIndex(5); AddIndex(1);

		//Bottom
		AddIndex(6); AddIndex(2); AddIndex(3);
		AddIndex(6); AddIndex(3); AddIndex(7);
	}else {
		//Front
		AddIndex(0);  AddIndex(2);  AddIndex(1);
		AddIndex(1); AddIndex(2); AddIndex(3);

		//Left
		AddIndex(4); AddIndex(6); AddIndex(0);
		AddIndex(0); AddIndex(6); AddIndex(2);

		//Right
		AddIndex(5); AddIndex(3); AddIndex(7);
		AddIndex(5); AddIndex(1); AddIndex(3);

		//Back
		AddIndex(6); AddIndex(4); AddIndex(5);
		AddIndex(6); AddIndex(5); AddIndex(7);

		//Top
		AddIndex(0); AddIndex(1); AddIndex(4);
		AddIndex(4); AddIndex(1); AddIndex(5);

		//Bottom
		AddIndex(6); AddIndex(3); AddIndex(2);
		AddIndex(6); AddIndex(7); AddIndex(3);
	}
}


void MeshBuilder::AddLathe(const Vec3f& axis, const Vec3f& arm1, const Vec3f& arm2, int slices, const std::vector<Point2f>& points, const RGBA& color, const float epsilon) {
	for (int iSlice = 0; iSlice <= slices; ++iSlice) {
		const float angle = (static_cast<float>(iSlice % slices) * MathUtils::TWO_PI / slices) + (MathUtils::TWO_PI / 8.0f), cosAngle = std::cos(angle), sinAngle = std::sin(angle);
		Matrix<float, 3, 3> mat = Matrix<float, 3, 3>::Identity();
		Vec3f radius = arm1 * cosAngle + arm2 * sinAngle;
		
		mat.value[0] = axis.x;
		mat.value[1] = axis.y;
		mat.value[2] = axis.z;

		mat.value[3] = radius.x;
		mat.value[4] = radius.y;
		mat.value[5] = radius.z;

		for (auto& p : points) {
			Vec3f newPoints = Transform(mat, Vec3f(p.x, p.y, 0.0f));
			SetPosition(newPoints + epsilon);
			//SetNormal(Vec3f());
			SetColor(color);
			vertices.push_back(stamp);
		}

		if (0 < iSlice) {
			for (uint32_t i = 1; i < (uint32_t)points.size(); ++i) {
				uint32_t i0 = (iSlice - 1) * uint32_t(points.size()) + (i - 1);
				uint32_t i1 = (iSlice - 0) * uint32_t(points.size()) + (i - 1);
				uint32_t i2 = (iSlice - 0) * uint32_t(points.size()) + (i - 0);
				uint32_t i3 = (iSlice - 1) * uint32_t(points.size()) + (i - 0);
				AddIndex(i0);
				AddIndex(i2);
				AddIndex(i1);

				AddIndex(i0);
				AddIndex(i3);
				AddIndex(i2);
			}
		}
	}
	//ComputeNormals();
}

void MeshBuilder::AddGrid(int xStart, int zStart, int xTileCount, int zTileCount, const Vec3f& positionOffset, const Vec3f& scale, const RGBA& color, const TerrainHeightImage& terrainContext) {
	int xVertexCount = xTileCount + 1;
	int zVertexCount = zTileCount + 1;
	
	int heightMapWidth = terrainContext.GetWidth();
	int heightMapLength = terrainContext.GetLength();
	for (int zVertex = zStart; zVertex < (zStart + zVertexCount); zVertex++) {
		for (int xVertex = xStart; xVertex < (xStart + xVertexCount); xVertex++) {
			SetColor(color);
			float height = terrainContext.GetHeight(xVertex, zVertex);
			SetPosition(xVertex * scale.x + positionOffset.x, height + positionOffset.y, zVertex * scale.z + positionOffset.z);
			SetUV(Vec2f(float(xVertex) / float(scale.x * 0.5f), float(zVertex) / float(scale.z * 0.5f)));
			
			Vec3f normal = Vec3f::UP;//terrainContext.GetHeightNormal(xVertex, zVertex);
			SetNormal(normal);
			vertices.push_back(stamp);
		}
	}


	int indexPointer = 0;
	for (int zTile = 0; zTile < zTileCount; zTile++) {
		for (int xTile = 0; xTile < xTileCount + 1; xTile++) {
			int leftTop = indexPointer++;
			int leftBottom = xVertexCount + leftTop;
			AddIndex(leftTop);
			AddIndex(leftBottom);
			if (leftTop == ((zTile + 1) * xVertexCount) - 1 && leftBottom < xVertexCount * zVertexCount - 1) {
				AddIndex(leftBottom);
				AddIndex(leftBottom - xTileCount);
			}

			Vec3f lt = vertices[leftTop].position;
			Vec3f lb = vertices[leftBottom].position;
			Vec3f rt = vertices[leftTop + 1].position;

			Vec3f edge1 = rt - lt;
			Vec3f edge2 = lb - lt;
			Vec3f normal = Normalize(Cross(edge2, edge1));
			vertices[leftTop].normal = normal;
		}
	}

	
}
void MeshBuilder::AddVertex(const Vec3f& _position) {
	stamp.position = _position;
	vertices.emplace_back(stamp);
	SetMaskBit(POSITION_BIT);
}

void MeshBuilder::AddIndex(int index) {
	if (vertices.size() <= index) {
		assert(false);
		return;
	}

	indices.emplace_back(index);
}

void MeshBuilder::CopyToMesh(Renderer& renderer, Mesh& mesh, VertexBufferBindCallback* bindFunction, VertexCopyCallback* copyFunction, unsigned int sizeofVertex, Mesh::TriangleType triangleType) {
	unsigned int vertexCount = vertices.size() - startIndex;
	if (vertexCount == 0) {
		return;
	}

	unsigned int vertexSize = sizeofVertex;
	unsigned int vertexBufferSize = vertexCount * vertexSize;

	byte* vertexBuffer = new byte[vertexBufferSize];
	byte* currentBufferIndex = vertexBuffer;
	for (unsigned int vertexIndex = startIndex; vertexIndex < vertexCount; vertexIndex++) {
		copyFunction(vertices[vertexIndex], currentBufferIndex);
		currentBufferIndex += vertexSize;
	}
	mesh.Initialize(renderer, bindFunction, vertexBuffer, vertexCount, sizeofVertex, indices.data(), indices.size());
	mesh.BuildTriangles(triangleType);

	delete[] vertexBuffer;
	vertexBuffer = nullptr;
}

void MeshBuilder::ComputeNormals() {
	static const double NORMAL_EPSILON = 0.0001;

	std::vector<uint32_t> uniqueVertIndices(vertices.size(), 0);
	for (uint32_t iVert = 0; iVert < uniqueVertIndices.size(); ++iVert) {
		if (uniqueVertIndices[iVert] == 0) {
			uniqueVertIndices[iVert] = iVert + 1;
			const Vec3f v0 = vertices[iVert].position;
			for (auto jVert = iVert + 1; jVert < vertices.size(); ++jVert) {
				const Vec3f v1 = vertices[jVert].position;
				if (Length(v1 - v0) < NORMAL_EPSILON) {
					uniqueVertIndices[jVert] = uniqueVertIndices[iVert];
				}
			}
		}
	}

	uint32_t idx0, idx1, idx2;
	

	for (int idx = 0; idx < vertices.size(); idx += 3) {
		idx0 = uniqueVertIndices[idx];
		idx1 = uniqueVertIndices[idx + 1];
		idx2 = uniqueVertIndices[idx + 2];

		VertexMaster v0 = vertices[idx0];
		VertexMaster v1 = vertices[idx1];
		VertexMaster v2 = vertices[idx2];

		const Vec3f normal = Cross(v1.position - v0.position, v2.position - v0.position);
		v0.normal += normal; v1.normal += normal; v2.normal += normal;
	}
	for (uint32_t iVert = 0; iVert < vertices.size(); ++iVert) {
		vertices[iVert].normal = vertices[uniqueVertIndices[iVert] - 1].normal;
	}
	for (auto& v : vertices) {
		v.normal = Normalize(v.normal);
	}
}

