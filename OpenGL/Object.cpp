#include "Mesh.h"
#include "Object.h"

Object::Object() 
	:pMesh(nullptr)
{
}

Object::~Object() {

}

bool Object::Initialize(OpenGL* pGL) {
	
	pMesh = new DiffuseMesh();
	if (!pMesh)
		return false;

	if(!pMesh->Initialize(*pGL)) {
		return false;
	}
	return true;
}

void Object::Shutdown(OpenGL* pGL) {
	pMesh->Shutdown(*pGL);
}

void Object::Render(OpenGL* pGL) {
	pMesh->Render(*pGL);
}

