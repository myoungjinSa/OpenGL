#pragma once
#include "Matrix.h"
#include "Renderer.h"
#include "RGBA.h"
#include <array>
#include <vector>


//the master Vertex. this is a superset of all possible vertex data.
struct VertexMaster {
	Vec3f position;
	RGBA color;
	Vec2f uv0;
	//Vec2f uv1;
	//Vec3f normal;
	//Vec3f tangent;
	//Vec3f bitangent;
	//Vec3f normal;

};
typedef unsigned char byte;
typedef void (VertexCopyCallback)(const VertexMaster& source, byte* destination);
typedef void (VertexBufferBindCallback)(const OpenGL& gl, void* pVertexBuffer, unsigned int vertexBufferId, unsigned int vertexCount, unsigned int sizeofVertex);

struct Vertex {
public:
	Vertex();
	Vertex(const Vec3f& pos);
	~Vertex();

	Vec3f position;
};
struct ColorVertex : public Vertex {
public:
	typedef unsigned int GLuint;

	ColorVertex();
	ColorVertex(const Vec3f& pos, const RGBA& color);
	~ColorVertex();

	static void Copy(const VertexMaster& source, byte* destination);
	static void BindVertexBuffer(const OpenGL& gl, void* pBuffer, unsigned int vertexBufferId, unsigned int vertexCount, unsigned int sizeofVertex);

	RGBA color;
};

struct TextureVertex : public Vertex {
public:
	TextureVertex();
	~TextureVertex();

	Vec2f uv;
};

class Shader;
class OpenGL;
class Mesh
{
	typedef unsigned int GLuint;
public:
	Mesh();
	Mesh(Mesh&& other) = delete;
	Mesh(const Mesh& other) = delete;
	virtual ~Mesh();

	bool Initialize(const OpenGL& gl, VertexBufferBindCallback* pBindFuction, void* vertexData, unsigned int numVertices, unsigned int sizeofVertex, unsigned int* indexData, unsigned int numIndices);
	void BindToVAO(const OpenGL& gl, Shader* pShader,GLuint vaoID);
	void Shutdown(const OpenGL& gl);
	void Render(const OpenGL& gl);

	Mesh& operator=(const Mesh& other) = delete;
	Mesh& operator=(Mesh&& other) = delete;

	Renderer::DrawMode drawMode;
protected:
	//bool InitializeBuffers(const OpenGL& gl);
	//void ShutdownBuffers(const OpenGL& gl);
	//void RenderBuffers(const OpenGL& gl);

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


	inline void SetMaskBit(const MeshDataFlag flag) {
		dataMask |= (1 << flag);
	};

	//QUERIES//////////////////////////////////////////////////////////////////////////
	inline bool IsInMask(const MeshDataFlag flag) { return ((dataMask & (1 << flag)) != 0); };

	void Begin();
	void End();

	void CopyToMesh(const OpenGL& gl, Mesh* mesh, VertexCopyCallback* copyFunction, unsigned int sizeofVertex);

	void AddCube(float sideLength, const RGBA& color);
	void AddQuad(const Vec3f& bottomLeft, const Vec3f& up, float upLength, const Vec3f& right, float rightLength, const RGBA& color = RGBA::WHITE, const Vec2f& uvOffset = Vec2f::ZERO, float uvStepSize = 1.0f);
	void AddVertex(const Vec3f& position);
	void AddIndex(int index);


	std::vector<VertexMaster> vertices;
	std::vector<unsigned int> indices;
	uint32_t dataMask;
private:
	VertexMaster stamp;
	unsigned int startIndex;
	Renderer::DrawMode drawMode;
};

