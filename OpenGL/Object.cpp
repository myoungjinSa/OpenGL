#include "Mesh.h"
#include "Object.h"

Object::Object() 
	:pMesh(nullptr)
{
}

Object::~Object() {

}

bool Object::Initialize(OpenGL* pGL) {
	//MeshBuilder Call
	MeshBuilder meshBuilder;
	meshBuilder.AddCube(2.0f, RGBA::BLUE);
	if(!pMesh)
		pMesh = new Mesh();
	
	if (!pMesh)
		return false;
	meshBuilder.CopyToMesh(*pGL, pMesh, &ColorVertex::Copy, sizeof(ColorVertex));

	return true;
}

void Object::Shutdown(OpenGL* pGL) {
	pMesh->Shutdown(*pGL);
}

void Object::Render(OpenGL* pGL) {
	pMesh->Render(*pGL);
}

