#pragma once
#include <vector>
#include <array>
#include "Compositor.h"
#include "Matrix.h"
#include "RGBA.h"

class Renderer; 
class Shader;
class OpenGL;
//the master Vertex. this is a superset of all possible vertex data.
struct VertexMaster {
	Vec3f position;
	RGBA color;
	Vec2f uv0;
	Vec3f normal;
	//Vec2f uv1;
	//Vec3f tangent;
	//Vec3f bitangent;
	//Vec3f normal;
};
typedef unsigned char byte;
typedef void (VertexCopyCallback)(const VertexMaster& source, byte* destination);
typedef void (VertexBufferBindCallback)(OpenGL& gl, void* pVertexBuffer, unsigned int vertexBufferId, unsigned int vertexCount, unsigned int sizeofVertex);
//struct Vertex {
//public:
//	Vertex();
//	Vertex(const Vec3f& pos);
//	~Vertex();
//
//	Vec3f position;
//};
//struct ColorVertex : public Vertex {
//public:
//	typedef unsigned int GLuint;
//
//	ColorVertex();
//	ColorVertex(const Vec3f& pos, const RGBA& color);
//	~ColorVertex();
//
//	static void Copy(const VertexMaster& source, byte* pDestination);
//	static void BindVertexBuffer(OpenGL& gl, void* pBuffer, unsigned int vertexBufferId, unsigned int vertexCount, unsigned int sizeofVertex);
//	
//	RGBA color;
//};

struct Vertex{
public:
	Vertex();
	Vertex(const Vec3f& pos, const Vec2f& uv, const Vec3f& normal);
	~Vertex();

	static void Copy(const VertexMaster& source, byte* pDestination);
	static void BindVertexBuffer(OpenGL& gl, void* pBuffer, unsigned int vertexBufferId, unsigned int vertexCount, unsigned int sizeOfVertex);
	static byte* ReadBufferData(void* pBuffer, size_t targetDataSize);

	Vec3f position;
	Vec2f uv0;
	Vec3f normal;
};

struct Triangle{
public:
	Triangle();
	Triangle(const Vertex& v0, const Vertex& v1, const Vertex& v2);
	~Triangle();

	void Set(const Vertex& v0, const Vertex& v1, const Vertex& v2);
	Vertex* Get(size_t index);

	void SetPosition(size_t index, float posX, float posY, float posZ);
	void SetPosition(size_t index, const Vec3f& pos);
	Vec3f GetPosition(size_t index) const;

	void SetNormal(size_t index, float normalX, float normalY, float normalZ);
	void SetNormal(size_t index, const Vec3f& normal);
	Vec3f GetNormal(size_t index) const;

	void SetUV(size_t index, float u, float v);
	void SetUV(size_t index, const Vec2f& uv);
	Vec2f GetUV(size_t index) const;

	std::array<Vertex, 3> vertices;
};


class Mesh
{
	typedef unsigned int GLuint;
public:
	Mesh();
	Mesh(Mesh&& other) = delete;
	Mesh(const Mesh& other) = delete;
	virtual ~Mesh();

	bool Initialize(Renderer& renderer, VertexBufferBindCallback* pBindFuction, void* vertexData, unsigned int numVertices, unsigned int sizeofVertex, unsigned int* indexData, unsigned int numIndices);
	void BindToVAO(const OpenGL& gl, Shader* pShader,GLuint vaoID);
	void Shutdown(Renderer& renderer);
	void Render(Renderer& renderer);

	bool BuildVertexList(void* vertexData);
	bool BuildIndexList(unsigned int* indicesData);
	bool BuildTriangleMeshes();

	Mesh& operator=(const Mesh& other) = delete;
	Mesh& operator=(Mesh&& other) = delete;
protected:
	std::vector<Vertex> vertexList;
	std::vector<unsigned int> indexList;

	std::vector<Triangle> meshes;
	int vertexCount, indexCount;
	unsigned int vertexArrayId, vertexBufferId, indexBufferId;
};



//
//class DiffuseMesh : public Mesh {
//public:
//	DiffuseMesh();
//	~DiffuseMesh() override;
//
//protected:
//	bool InitializeBuffers(const OpenGL& gl) override;
//	void ShutdownBuffers(const OpenGL& gl) override;
//	void RenderBuffers(const OpenGL& gl) override;
//
//	ColorVertex* vertices;
//	unsigned int* indices;
//};



class MeshBuilder {

public:
	enum MeshDataFlag {
		POSITION_BIT = 0,
		COLOR_BIT,
		UV0_BIT,
		UV1_BIT,
		NORMAL_BIT,
		TANGENT_BIT,
		BITANGENT_BIT,
		NORMALIZED_GLYPH_POSITION_BIT,
		NORMALIZED_STRING_POSITION_BIT,
		NORMALIZED_FRAG_POSITION_BIT,
		BONE_WEIGHTS_BIT,
		BONE_INDICES_BIT,
		NUM_MESH_DATA
	};
	struct PlaneData {
		PlaneData() {};
		PlaneData(const Vec3f& _initialPosition, const Vec3f& _right, const Vec3f& _up)
			: initialPosition(_initialPosition), right(_right), up(_up) {}
		Vec3f initialPosition;
		Vec3f right;
		Vec3f up;
	};

	MeshBuilder();

	//SETTERS///////////////////////////////////////////////////////////////////
	inline void SetColor(const RGBA& color) { stamp.color = color; SetMaskBit(COLOR_BIT); };
	inline void SetUV(const Vec2f& uv) { stamp.uv0 = uv; SetMaskBit(UV0_BIT); };
	inline void SetUV(float u, float v) { stamp.uv0 = Vec2f(u, v); SetMaskBit(UV0_BIT); };
	inline void SetNormal(const Vec3f& normal) { stamp.normal = normal, SetMaskBit(NORMAL_BIT); };

	inline void SetMaskBit(const MeshDataFlag flag) {
		dataMask |= (1 << flag);
	};

	//QUERIES//////////////////////////////////////////////////////////////////////////
	inline bool IsInMask(const MeshDataFlag flag) { return ((dataMask & (1 << flag)) != 0); };

	void Begin();
	void End();

	void CopyToMesh(Renderer& renderers, Mesh* mesh, VertexCopyCallback* copyFunction, unsigned int sizeofVertex);

	void AddCube(float sideLength, const RGBA& color);
	void AddQuad(const Vec3f& bottomLeft, const Vec3f& up, float upLength, const Vec3f& right, float rightLength, const Vec3f& normal, const RGBA& color = RGBA::WHITE, const Vec2f& uvOffset = Vec2f::ZERO, float uvStepSize = 1.0f);
	void AddVertex(const Vec3f& position);
	void AddIndex(int index);


	std::vector<VertexMaster> vertices;
	std::vector<unsigned int> indices;
	uint32_t dataMask;
private:
	VertexMaster stamp;
	unsigned int startIndex;
};


