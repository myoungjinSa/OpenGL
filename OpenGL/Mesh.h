#pragma once
#include "Matrix.h"
#include <array>
struct Vertex {
public:
	Vertex();
	~Vertex();

	Vec3f position;
};
struct ColorVertex : public Vertex {
public:
	ColorVertex();
	~ColorVertex();

	Vec3f color;
};

struct TextureVertex : public Vertex {
public:
	TextureVertex();
	~TextureVertex();

	Vec2f uv;
};

//the master Vertex. this is a superset of all possible vertex data.
struct Vectex_Master {
	Vec3f position;
	Vec3f tangent;
	Vec3f bitangent;
	Vec3f normal;

};

class OpenGL;
class Mesh
{
public:
	Mesh();
	virtual ~Mesh();

	bool Initialize(const OpenGL& gl);
	void Shutdown(const OpenGL& gl);
	void Render(const OpenGL& gl);
protected:
	virtual bool InitializeBuffers(const OpenGL& gl);
	virtual void ShutdownBuffers(const OpenGL& gl);
	virtual void RenderBuffers(const OpenGL& gl);

	Vec3f position;
	int vertexCount, indexCount;
	unsigned int vertexArrayId, vertexBufferId, indexBufferId;
};

class DiffuseMesh : public Mesh {
public:
	DiffuseMesh();
	~DiffuseMesh() override;

protected:
	bool InitializeBuffers(const OpenGL& gl) override;
	void ShutdownBuffers(const OpenGL& gl) override;
	void RenderBuffers(const OpenGL& gl) override;

	ColorVertex* vertices;
	unsigned int* indices;
};



class MeshBuilder {

public:
	enum MeshData {
		POSITION_BIT = 0,
		COLOR_BIT,
		UV0_BIT,
		UV1_BIT,
		NORMALIZED_GLYPH_POSITION_BIT,
		NORMALIZED_STRING_POSITION_BIT,
		NORMALIZED_FRAG_POSITION_BIT,
		BONE_WEIGHTS_BIT,
		BONE_INDICES_BIT,
		TANGENT_BIT,
		BITANGENT_BIT,
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


	void Begin();
	void End();


private:

	unsigned int startIndex;

};

