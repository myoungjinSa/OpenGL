#pragma once
class OpenGL;
class Mesh;
class Object
{
public:
	Object();
	~Object();

	bool Initialize(OpenGL* pGL);
	void Shutdown(OpenGL* pGL);
	void Render(OpenGL* pGL);
private:
	Mesh* pMesh;
};

