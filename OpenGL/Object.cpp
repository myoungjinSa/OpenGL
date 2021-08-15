#include "Common.h"
#include "Mesh.h"
#include "Texture.h"
#include "Object.h"
#include "Input.h"
#include "Material.h"
#include "RayCast.h"
#include "Logger.h"

Object::Object() 
	: pMesh(nullptr), albedoMap(nullptr), normalMap(nullptr)
{
	transform = AddComponent<class Transform>();
}

Object::~Object() {

}

void Object::SetPosition(float x, float y, float z) {
	transform.get()->SetPosition(x, y, z);
}

void Object::SetPosition(const Vec3f& _position) {
	transform.get()->SetPosition(_position);
}

Vec3f Object::GetPosition() const {
	return transform.get()->GetPosition();
}

Vec3f Object::GetLook() const {
	return transform.get()->GetLook();
}

Vec3f Object::GetRight() const {
	return transform.get()->GetRight();
}

Vec3f Object::GetUp() const {
	return transform.get()->GetUp();
}

void Object::Move(const Vec3f& dir, float movingSpeed, float elapsedTime) {
	std::weak_ptr<class Transform> weak_tr = transform;
	{
		std::shared_ptr<class Transform> shared_tr = weak_tr.lock();
		if (shared_tr) {
			shared_tr->SetMovingSpeed(movingSpeed);
			shared_tr->Move(dir, elapsedTime);
		}
	}
}

void Object::Rotate(float pitch, float yaw, float roll) {
	transform.get()->Rotate(pitch, yaw, roll);
}

bool Object::Initialize(Renderer& renderer) {
	return true;
}

bool Object::Intersect(const Ray& ray, double& distance) {
	bool bIntersect = false;
	for (size_t iTriangle = 0; iTriangle < pMesh->GetTriangleMeshCount(); iTriangle++) {
		Triangle triangleMesh = pMesh->GetTriangleMesh(iTriangle);
		bIntersect = IntersectTriangle(ray, triangleMesh.vertices[0].position, triangleMesh.vertices[1].position, triangleMesh.vertices[2].position, false, distance);
		if (bIntersect)
			LogDebug(L"Hit\n");
	}
	
	return bIntersect;
}
bool Object::IntersectTriangle(const Ray& ray, const Vec3f& v0, const Vec3f& v1, const Vec3f& v2, bool bFrontOnly, double& distance) {
	Vec3f edge1 = v1 - v0;
	Vec3f edge2 = v2 - v0;

	Vec3f pVec = ray.GetDirection().Cross(edge2);

	float det = edge1.DotProduct(pVec);

	Vec3f tVec;
	if (det > 0) {
		tVec = v0 - ray.GetPosition();
	}else {
		//BackFace 
		if (bFrontOnly)
			return false;

		tVec = v0 - ray.GetPosition();
		det = -det;
	}

	if (det < 0.0001f)
		return false;


	float u = tVec.DotProduct(pVec);
	if (u < 0 || u > det)
		return false;

	Vec3f qVec = tVec.Cross(edge1);

	float v = ray.GetDirection().DotProduct(qVec);
	if (v < 0 || u + v > det)
		return false;

	distance = edge2.DotProduct(qVec) / det;

	return true;
}



///////////////////////////////////////////////////////////////
Cube::Cube()
	:Object()
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

bool Cube::Initialize(Renderer& renderer) {
	Object::Initialize(renderer);

	//MeshBuilder Call
	MeshBuilder meshBuilder;
	meshBuilder.AddCube(transform.get()->GetPosition(), Vec3f(1.0f, 1.0f, 1.0f), RGBA::BLUE);

	if (!pMesh)
		pMesh = std::make_shared<Mesh>();

	if (!pMesh)
		return false;

	meshBuilder.CopyToMesh(renderer, pMesh.get(), &Vertex::Copy, sizeof(Vertex));

	albedoMap = TextureLoader::GetTexture(renderer, "Capture.bmp");
	//normalMap = TextureLoader::GetTexture(renderer, "Resource\\Texture\\BMP\\NormalMap.bmp");

	Vec3f diffuseColor(0.8f, 0.85f, 0.85f);
	Vec4f ambientColor(0.3f, 0.3f, 0.3f, 1.0f);
	Vec3f specularColor(1.0f, 1.0f, 1.0f);
	material = std::make_shared<Material>(diffuseColor, ambientColor, specularColor, std::make_pair(Material::TextureType::TEXTURE_ALBEDO, albedoMap->textureID));
	//material.get()->SetTextureMap(std::make_pair(Material::TextureType::TEXTURE_NORMAL, normalMap->textureID));

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


Sphere::Sphere(float _radius, int _stackCount, int _sectorCount)
	:Object(), radius(_radius), stackCount(_stackCount), sectorCount(_sectorCount)
{

}

Sphere::Sphere(const Sphere& other)
{
	operator=(other);
}



Sphere::~Sphere() {

}
Sphere& Sphere::operator=(const Sphere& other) {
	if (this == &other)
		return *this;

	if (0 < other.pMesh.use_count()) {
		pMesh = other.pMesh;
	}

	return *this;
}

bool Sphere::Initialize(Renderer& renderer) {
	Object::Initialize(renderer);

	//MeshBuilder Call
	MeshBuilder meshBuilder;
	meshBuilder.AddSphere(transform.get()->GetPosition(), radius, sectorCount, stackCount);

	if (!pMesh)
		pMesh = std::make_shared<Mesh>();

	if (!pMesh)
		return false;

	meshBuilder.CopyToMesh(renderer, pMesh.get(), &Vertex::Copy, sizeof(Vertex));

	albedoMap = TextureLoader::GetTexture(renderer, "Capture.bmp");

	Vec3f diffuseColor(0.8f, 0.85f, 0.85f);
	Vec4f ambientColor(0.3f, 0.3f, 0.3f, 1.0f);
	Vec3f specularColor(1.0f, 1.0f, 1.0f);
	material = std::make_shared<Material>(diffuseColor, ambientColor, specularColor, std::make_pair(Material::TextureType::TEXTURE_ALBEDO, albedoMap->textureID));

	return true;
}

void Sphere::Shutdown(Renderer& renderer) {
	Object::Shutdown(renderer);
	pMesh->Shutdown(renderer);
}

void Sphere::Update(float deltaTime) {

}

void Sphere::Render(Renderer& renderer) {
	Object::Render(renderer);
	pMesh->Render(renderer);
}

