#pragma once
class OpenGL;
class Mesh;
class Object
{
public:
	Object();
	~Object();

	virtual bool Initialize(OpenGL* pGL);
	virtual void Shutdown(OpenGL* pGL);
	virtual void Render(OpenGL* pGL);
private:
	Mesh* pMesh;
};

class Triangle : public Object {

};