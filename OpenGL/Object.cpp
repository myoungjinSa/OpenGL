#include "Mesh.h"
#include "Object.h"

Object::Object() 
	:pMesh{nullptr}
{
}

Object::Object(const Object& other) {
	operator=(other);
}

Object::~Object() {

}

Object& Object::operator=(const Object& other) {
	if (this == &other)
		return *this;
	
	if (0 < other.pMesh.use_count()) {
		pMesh = other.pMesh;
	}

	return *this;
}

bool Object::Initialize(OpenGL* pGL) {
	//MeshBuilder Call
	MeshBuilder meshBuilder;
	meshBuilder.AddCube(2.0f, RGBA::BLUE);
	if(!pMesh)
		pMesh = std::make_shared<Mesh>();
	

	if (!pMesh)
		return false;
	meshBuilder.CopyToMesh(*pGL, pMesh.get(), &ColorVertex::Copy, sizeof(ColorVertex));

	return true;
}

void Object::Shutdown(OpenGL* pGL) {
	pMesh->Shutdown(*pGL);
}

void Object::Render(OpenGL* pGL) {
	pMesh->Render(*pGL);
}
