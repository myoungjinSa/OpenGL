#include "Common.h"
#include "Mesh.h"
#include "Texture.h"
#include "Object.h"
#include "Input.h"
#include "Material.h"
#include "RayCast.h"

Object::Object() 
	: pMesh{ nullptr }, texture{ nullptr }, position(0.0f, 0.0f, 0.0f), rotation(0.0f, 0.0f, 0.0f), movingSpeed(1.0f)
{

}

void Object::SetPosition(const Vec3f& _position) {
	position = _position;
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

void Object::Move(const Vec3f& direction, float elapsedTime) {
	position = direction * elapsedTime * movingSpeed;
}

Vec3f Object::GetLook() const {
	Vec3f lookVector(worldMatrix.value[8], worldMatrix.value[9], worldMatrix.value[10]);
	return Normalize(lookVector);
}

Vec3f Object::GetRight() const {
	Vec3f rightVector(worldMatrix.value[4], worldMatrix.value[5], worldMatrix.value[6]);
	return Normalize(rightVector);
}

Vec3f Object::GetUp() const {
	Vec3f upVector(worldMatrix.value[0], worldMatrix.value[1], worldMatrix.value[2]);
	return Normalize(upVector);
}

bool Object::Intersect(const Ray& ray, double& distance) {
	bool bIntersect = false;
	for (size_t iTriangle = 0; iTriangle < pMesh->GetTriangleMeshCount(); iTriangle++) {
		Triangle triangleMesh = pMesh->GetTriangleMesh(iTriangle);
		bIntersect = IntersectTriangle(ray, triangleMesh.vertices[0].position, triangleMesh.vertices[1].position, triangleMesh.vertices[2].position, distance);
	}
	
	return bIntersect;
}
bool Object::IntersectTriangle(const Ray& ray, const Vec3f& v0, const Vec3f& v1, const Vec3f& v2, double& distance) {

	return true;
}
///////////////////////////////////////////////////////////////
Cube::Cube()
	:Object()
{
	worldMatrix = Matrix<float, 4, 4>::Identity();
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

bool Cube::Initialize(Renderer& renderer) {
	Object::Initialize(renderer);

	//MeshBuilder Call
	MeshBuilder meshBuilder;
	meshBuilder.AddCube(2.0f, RGBA::BLUE);
	if (!pMesh)
		pMesh = std::make_shared<Mesh>();

	if (!pMesh)
		return false;

	meshBuilder.CopyToMesh(renderer, pMesh.get(), &Vertex::Copy, sizeof(Vertex));

	texture = TextureLoader::GetTexture(renderer, "Capture.bmp");

	Vec3f diffuseColor(0.8f, 0.85f, 0.85f);
	Vec4f ambientColor(0.3f, 0.3f, 0.3f, 1.0f);
	Vec3f specularColor(1.0f, 1.0f, 1.0f);
	material = std::make_shared<Material>(diffuseColor, ambientColor, specularColor, texture->textureID);

	return true;
}

void Cube::Shutdown(Renderer& renderer) {
	Object::Shutdown(renderer);
	pMesh->Shutdown(renderer);
}

void Cube::Update(float deltaTime) {

}

void Cube::Render(Renderer& renderer) {
	Object::Render(renderer);
	pMesh->Render(renderer);
}

