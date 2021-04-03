#pragma once
#include "Matrix.h"

class Vertex {
public:
	Vertex();
	virtual ~Vertex();

	Vec3f position;
};
class ColorVertex : public Vertex {
public:
	ColorVertex();
	virtual ~ColorVertex();

	Vec3f color;
};

class OpenGL;
class Mesh
{
public:
	Mesh();
	virtual ~Mesh();

	bool Initialize(const OpenGL& pGL);
	void Shutdown(const OpenGL& pGL);
	void Render(const OpenGL& pGL);
private:
	bool InitializeBuffers(const OpenGL& pGL);
	void ShutdownBuffers(const OpenGL& pGL);
	void RenderBuffers(const OpenGL& pGL);

	ColorVertex* vertices;
	unsigned int* indices;
	int vertexCount, indexCount;
	unsigned int vertexArrayId, vertexBufferId, indexBufferId;
};


