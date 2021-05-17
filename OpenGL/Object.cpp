#include "Mesh.h"
#include "Texture.h"
#include "Object.h"

Object::Object() 
	:position(0.0f, 0.0f, 0.0f), rotation(0.0f, 0.0f, 0.0f)
{

}
void Object::SetPosition(float x, float y, float z) {
	position = Vec3f(x, y, z);
}

Vec3f Object::GetPosition() const {
	return position;
}
void Object::GetPosition(std::array<float, 4>& _position) {
	_position[0] = position.x;
	_position[1] = position.y;
	_position[2] = position.z;
	_position[3] = 1.0f;
}

void Object::SetRotation(float x, float y, float z) {
	rotation = Vec3f(x, y, z);
}

Vec3f Object::GetRotation() const {
	return rotation;
}
///////////////////////////////////////////////////////////////
Cube::Cube()
	:Object(), pMesh{ nullptr }
{

}

Cube::Cube(const Cube& other)
{
	operator=(other);
}

Cube::~Cube() {

}

Cube& Cube::operator=(const Cube& other) {
	if (this == &other)
		return *this;

	if (0 < other.pMesh.use_count()) {
		pMesh = other.pMesh;
	}

	return *this;
}

bool Cube::Initialize(OpenGL& gl) {
	Object::Initialize(gl);

	//MeshBuilder Call
	MeshBuilder meshBuilder;
	meshBuilder.AddCube(2.0f, RGBA::BLUE);
	if (!pMesh)
		pMesh = std::make_shared<Mesh>();

	if (!pMesh)
		return false;
	//meshBuilder.CopyToMesh(*pGL, pMesh.get(), &ColorVertex::Copy, sizeof(ColorVertex));
	meshBuilder.CopyToMesh(gl, pMesh.get(), &TexturedVertex::Copy, sizeof(TexturedVertex));

	texture = TextureLoader::GetTexture(gl, "Capture.bmp");
	return true;
}

void Cube::Shutdown(OpenGL& gl) {
	Object::Shutdown(gl);
	pMesh->Shutdown(gl);
}

void Cube::Update(float deltaTime) {

}

void Cube::Render(OpenGL& gl) {
	Object::Render(gl);
	pMesh->Render(gl);
}



