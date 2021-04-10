#pragma once
#include "Matrix.h"

class Vertex {
public:
	Vertex();
	~Vertex();

	Vec3f position;
};
class ColorVertex : public Vertex {
public:
	ColorVertex();
	~ColorVertex();

	Vec3f color;
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
	
	ColorVertex* vertices;
	unsigned int* indices;
	int vertexCount, indexCount;
	unsigned int vertexArrayId, vertexBufferId, indexBufferId;
};

class TriangleMesh : public Mesh {
public:
	TriangleMesh();
	~TriangleMesh() override;

protected:
	bool InitializeBuffers(const OpenGL& gl) override;
	void ShutdownBuffers(const OpenGL& gl) override;
	void RenderBuffers(const OpenGL& gl) override;
};

