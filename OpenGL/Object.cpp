#include "Mesh.h"
#include "Texture.h"
#include "Object.h"


Object::Object() 
	:pMesh{nullptr}
{
}

Object::Object(const Object& other) {
	operator=(other);
}

Object::~Object() {
	//ReleaseTexture();
}

Object& Object::operator=(const Object& other) {
	if (this == &other)
		return *this;
	
	if (0 < other.pMesh.use_count()) {
		pMesh = other.pMesh;
	}

	return *this;
}

bool Object::Initialize(OpenGL& gl) {
	//MeshBuilder Call
	MeshBuilder meshBuilder;
	meshBuilder.AddCube(2.0f, RGBA::BLUE);
	if(!pMesh)
		pMesh = std::make_shared<Mesh>();
	
	if (!pMesh)
		return false;
	//meshBuilder.CopyToMesh(*pGL, pMesh.get(), &ColorVertex::Copy, sizeof(ColorVertex));
	meshBuilder.CopyToMesh(gl, pMesh.get(), &TexturedVertex::Copy, sizeof(TexturedVertex));

	texture = TextureLoader::GetTexture(gl, "Capture.bmp");

	return true;
}

void Object::Shutdown(OpenGL& gl) {
	pMesh->Shutdown(gl);
}

void Object::Render(OpenGL& gl) {
	pMesh->Render(gl);
}
