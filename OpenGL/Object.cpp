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

bool Object::Initialize(OpenGL* pGL) {
	//MeshBuilder Call
	MeshBuilder meshBuilder;
	meshBuilder.AddCube(2.0f, RGBA::BLUE);
	if(!pMesh)
		pMesh = std::make_shared<Mesh>();
	
	if (!pMesh)
		return false;
	//meshBuilder.CopyToMesh(*pGL, pMesh.get(), &ColorVertex::Copy, sizeof(ColorVertex));
	meshBuilder.CopyToMesh(*pGL, pMesh.get(), &TexturedVertex::Copy, sizeof(TexturedVertex));

	texture = TextureLoader::GetTexture(*pGL, "note.txt");
	//LoadTexture(pGL, "../Resource/Texture/Ice", )

	return true;
}

void Object::Shutdown(OpenGL* pGL) {
	pMesh->Shutdown(*pGL);
}

void Object::Render(OpenGL* pGL) {
	pMesh->Render(*pGL);
}
//
//bool Object::LoadTexture(OpenGL* pGL, const char* textureFileName, unsigned int textureUnit, bool wrap) {
//	bool result;
//
//	pTexture = new Texture();
//	if (!pTexture)
//		return false;
//
//	result = pTexture->Initialize(pGL, textureFileName, textureUnit, wrap);
//	if (!result)
//		return false;
//
//	return true;
//}


//void Object::ReleaseTexture() {
//	if (pTexture) {
//		pTexture->Shutdown();
//		delete pTexture;
//		pTexture = nullptr;
//	}
//}