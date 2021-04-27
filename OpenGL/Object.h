#pragma once
#include <memory>
class OpenGL;
class Mesh;
class Texture;
class Object
{
public:
	Object();
	Object(const Object& other);
	Object(Object&& other)noexcept;
	~Object();


	virtual bool Initialize(OpenGL* pGL);
	virtual void Shutdown(OpenGL* pGL);
	virtual void Render(OpenGL* pGL);

	Object& operator=(const Object& other);
	Object& operator=(Object&& other)noexcept;

private:
	//Mesh* pMesh;
	std::shared_ptr<Mesh> pMesh;
	std::shared_ptr<Texture> texture;
};

class Triangle : public Object {

};

