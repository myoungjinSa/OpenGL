#include "Common.h"
#include "Mesh.h"
#include "Texture.h"
#include "Object.h"
#include "Input.h"
#include "Material.h"
#include "RayCast.h"
#include "Logger.h"
#include "BoundingVolume.h"
#include "Renderer.h"
#include "Shader.h"
#include "Light.h"
#include "Camera.h"

void MakeWorldMatrix(const Vec3f& position, const Vec3f& look, const Vec3f& right, const Vec3f& up, Matrix<float, 4, 4>& worldMatrix) {
	//Right
	worldMatrix.value[0] = right.x;
	worldMatrix.value[1] = right.y;
	worldMatrix.value[2] = right.z;

	//Up
	worldMatrix.value[4] = up.x;
	worldMatrix.value[5] = up.y;
	worldMatrix.value[6] = up.z;

	//Look
	worldMatrix.value[8] = look.x;
	worldMatrix.value[9] = look.y;
	worldMatrix.value[10] = look.z;

	//Position
	worldMatrix.value[12] = position.x;
	worldMatrix.value[13] = position.y;
	worldMatrix.value[14] = position.z;
	worldMatrix.value[15] = 1.0f;
}

GameObject::GameObject() 
	: pMesh(nullptr), albedoMap(nullptr), normalMap(nullptr)
{
	transform = AddComponent<RigidTransform>();
}

GameObject::~GameObject() {

}

void GameObject::SetPosition(float x, float y, float z) {
	transform.get()->SetPosition(x, y, z);
}

void GameObject::SetPosition(const Vec3f& _position) {
	transform.get()->SetPosition(_position);
}

Vec3f GameObject::GetPosition() const {
	return transform.get()->GetPosition();
}

Vec3f GameObject::GetLook() const {
	return transform.get()->GetLook();
}

Vec3f GameObject::GetRight() const {
	return transform.get()->GetRight();
}

Vec3f GameObject::GetUp() const {
	return transform.get()->GetUp();
}

void GameObject::Move(const Vec3f& dir, float movingSpeed, float elapsedTime) {
	std::weak_ptr<RigidTransform> weak_tr = transform;
	{
		std::shared_ptr<RigidTransform> shared_tr = weak_tr.lock();
		if (shared_tr) {
			shared_tr->SetMovingSpeed(movingSpeed);
			shared_tr->Move(dir, elapsedTime);
		}
	}
}

void GameObject::Rotate(float pitch, float yaw, float roll) {
	transform.get()->Rotate(pitch, yaw, roll);
}

bool GameObject::Initialize(Renderer& renderer) {
	return false;
}

bool GameObject::Intersect(const Ray& ray, double& distance) {
	
	for (size_t iTriangle = 0; iTriangle < pMesh->GetTriangleMeshCount(); iTriangle++) {
		Triangle triangleMesh = pMesh->GetTriangleMesh(iTriangle);
		Vec4f vertex0 = ::Transform(transform->GetWorldMatrix(), Vec4f(triangleMesh.vertices[0].position, 1.0f));
		Vec4f vertex1 = ::Transform(transform->GetWorldMatrix(), Vec4f(triangleMesh.vertices[1].position, 1.0f));
		Vec4f vertex2 = ::Transform(transform->GetWorldMatrix(), Vec4f(triangleMesh.vertices[2].position, 1.0f));

		if (IntersectTriangle(ray, Vec3f(vertex0.x, vertex0.y, vertex0.z), Vec3f(vertex1.x, vertex1.y, vertex1.z), Vec3f(vertex2.x, vertex2.y, vertex2.z), distance)) {
			return true;
		}
	}
	
	return false;
}
bool GameObject::IntersectTriangle(const Ray& ray, const Vec3f& v0, const Vec3f& v1, const Vec3f& v2, bool bFrontOnly, double& distance) {
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

bool GameObject::IntersectTriangle(const Ray& ray, const Vec3f& v0, const Vec3f& v1, const Vec3f& v2, double& distance) {
	auto e1 = v1 - v0, e2 = v2 - v0, h = Cross(ray.GetDirection(), e2);
	auto a = DotProduct(e1, h);
	if (std::abs(a) == 0) return false;

	float f = 1 / a;
	auto s = ray.GetPosition() - v0;
	auto u = f * DotProduct(s, h);
	if (u < 0 || u > 1) return false;

	auto q = Cross(s, e1);
	auto v = f * DotProduct(ray.GetDirection(), q);
	if (v < 0 || u + v > 1) return false;

	auto t = f * DotProduct(e2, q);
	if (t < 0) return false;

	distance = t;
	
	return true;
}

bool GameObject::IntersectTriangle(const Ray& ray, const Triangle& triangle, double& distance) {
	Vec3f v0 = triangle.GetPosition(0);
	Vec3f v1 = triangle.GetPosition(1);
	Vec3f v2 = triangle.GetPosition(2);

	auto e1 = v1 - v0;
	auto e2 = v2 - v0;
	auto h = Cross(ray.GetDirection(), e2);
	
	auto a = DotProduct(e1, h);
	if (std::abs(a) == 0)
		return false;

	float f = 1 / a;
	auto s = ray.GetPosition() - v0;
	auto u = f * DotProduct(s, h);
	if (u < 0 || u > 1) return false;

	auto q = Cross(s, e1);
	auto v = f * DotProduct(ray.GetDirection(), q);
	if (v < 0 || u + v > 1) return false;

	auto t = f * DotProduct(e2, q);
	if (t < 0) return false;

	distance = t;

	return true;
}

void GameObject::FillShaderParameter(ShaderParameter& shaderParam, const Matrix<float, 4, 4>& viewMatrix, const Matrix<float, 4, 4>& projectionMatrix, const Light& light, const Camera& Camera) {
	Matrix<float, 4, 4> worldMatrix = Matrix<float, 4, 4>::Identity();
	MakeWorldMatrix(GetPosition(), GetLook(), GetRight(), GetUp(), worldMatrix);
	shaderParam.worldMatrix = worldMatrix;
	shaderParam.viewMatrix = viewMatrix;
	shaderParam.projectionMatrix = projectionMatrix;

	shaderParam.lightPosition = light.GetPosition();
	shaderParam.diffuseAlbedo = material->GetDiffuseAlbedo();
	shaderParam.ambientAlbedo = material->GetAmbientAlbedo();
	shaderParam.specularAlbedo = material->GetSpecularAlbedo();

	shaderParam.cameraPosition = Camera.GetPosition();

	shaderParam.textureUnit = material->GetTextureUnit(Material::TextureType::TEXTURE_ALBEDO);
}


///////////////////////////////////////////////////////////////
Cube::Cube()
	:GameObject(), extent(1.0f, 1.0f, 1.0f)
{

}

Cube::Cube(const Vec3f& size)
	: GameObject(), extent(size)
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
	GameObject::Initialize(renderer);

	shader = std::make_shared<TextureShader>(this);
	if (!shader)
		return false;

	if (!shader->Initialize(renderer)) {
		LogError(L"Could not initialize the Default Shader\n");
		return false;
	}

	//MeshBuilder Call
	MeshBuilder meshBuilder;
	meshBuilder.AddCube(transform.get()->GetPosition(), extent, RGBA::BLUE);

	if (!pMesh)
		pMesh = std::make_shared<Mesh>();

	if (!pMesh)
		return false;

	meshBuilder.CopyToMesh(renderer, pMesh.get(), &Vertex::BindVertexBuffer, &Vertex::Copy, sizeof(Vertex));

	albedoMap = TextureLoader::GetTexture(renderer, L"IU-Celebrity.webm");
	//normalMap = TextureLoader::GetTexture(renderer, "Resource\\Texture\\BMP\\NormalMap.bmp");
	
	Vec3f diffuseColor(0.8f, 0.85f, 0.85f);
	Vec4f ambientColor(0.3f, 0.3f, 0.3f, 1.0f);
	Vec3f specularColor(1.0f, 1.0f, 1.0f);
	material = std::make_shared<Material>(diffuseColor, ambientColor, specularColor, std::make_pair(Material::TextureType::TEXTURE_ALBEDO, albedoMap->textureID));

	AddComponent<BoundingBox>();
	auto boundingBox = GetComponent<BoundingBox>();
	
	boundingBox->SetCenter(transform->GetPosition());
	boundingBox->SetExtent(GetExtent());

	boundingVolume = boundingBox;
	if (!boundingVolume->Init(renderer))
		return false;

	return true;
}

void Cube::Shutdown(Renderer& renderer) {
	GameObject::Shutdown(renderer);
	if(shader)
		shader->Shutdown(renderer);

	pMesh->Shutdown(renderer);
}

void Cube::Update(float deltaTime) {
	albedoMap->Update(deltaTime);
}

void Cube::Render(Renderer& renderer, const ShaderParameter& shaderParam) {	
	GameObject::Render(renderer, shaderParam);
	
	shader->Render(renderer, shaderParam);
	renderer.SetDrawMode(Renderer::DrawMode::TRIANGLES);
	renderer.SetDepthTest(true);

	pMesh->Render(renderer);
	boundingVolume->Render(renderer, shaderParam.viewMatrix, shaderParam.projectionMatrix);
}


Vec3f Cube::GetExtent() const {
	float maxX = 0.0, maxY = 0.0, maxZ = 0.0;

	for (size_t iMesh = 0; iMesh < pMesh->GetTriangleMeshCount(); iMesh++) {
		Triangle meshes = pMesh->GetTriangleMesh(iMesh);
		for (size_t iVertex = 0; iVertex < 3; iVertex++) {
			Vec3f pos = meshes.GetPosition(iVertex);
			if (abs(maxX) < abs(pos.x))
				maxX = pos.x;
			if (abs(maxY) < abs(pos.y))
				maxY = pos.y;
			if (abs(maxZ) < abs(pos.z))
				maxZ = pos.z;
		}
	}
	return Vec3f(maxX, maxY, maxZ);
}


Sphere::Sphere(float _radius, int _stackCount, int _sectorCount)
	:GameObject(), radius(_radius), stackCount(_stackCount), sectorCount(_sectorCount)
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
	GameObject::Initialize(renderer);

	shader = std::make_shared<TextureShader>(this);
	if (!shader)
		return false;

	if (!shader->Initialize(renderer)) {
		LogError(L"Could not initialize the Default Shader\n");
		return false;
	}

	//MeshBuilder Call
	MeshBuilder meshBuilder;
	meshBuilder.AddSphere(transform.get()->GetPosition(), radius, sectorCount, stackCount);

	if (!pMesh)
		pMesh = std::make_shared<Mesh>();

	if (!pMesh)
		return false;

	meshBuilder.CopyToMesh(renderer, pMesh.get(), &Vertex::BindVertexBuffer, &Vertex::Copy, sizeof(Vertex));
	albedoMap = TextureLoader::GetTexture(renderer, L"¿¡½ºÆÄ.webm");


	Vec3f diffuseColor(0.8f, 0.85f, 0.85f);
	Vec4f ambientColor(0.3f, 0.3f, 0.3f, 1.0f);
	Vec3f specularColor(1.0f, 1.0f, 1.0f);
	material = std::make_shared<Material>(diffuseColor, ambientColor, specularColor, std::make_pair(Material::TextureType::TEXTURE_ALBEDO, albedoMap->textureID));

	AddComponent<BoundingBox>();
	auto boundingBox = GetComponent<BoundingBox>();

	boundingBox->SetCenter(transform->GetPosition());
	boundingBox->SetExtent(GetExtent());

	boundingVolume = boundingBox;
	if (!boundingVolume->Init(renderer)) {
		return false;
	}

	return true;
}

void Sphere::Shutdown(Renderer& renderer) {
	GameObject::Shutdown(renderer);
	if (shader)
		shader->Shutdown(renderer);
	pMesh->Shutdown(renderer);
}

void Sphere::Update(float deltaTime) {
	albedoMap->Update(deltaTime);
}

void Sphere::Render(Renderer& renderer, const ShaderParameter& shaderParam) {
	GameObject::Render(renderer, shaderParam);
	shader->Render(renderer, shaderParam);
	renderer.SetDrawMode(Renderer::DrawMode::TRIANGLES);
	renderer.SetDepthTest(true);
	pMesh->Render(renderer);
	boundingVolume->Render(renderer, shaderParam.viewMatrix, shaderParam.projectionMatrix);
}



///////////////////////////////////////////////////
Cylinder::Cylinder(const Vec3f& _axis, const Vec3f& _arm1, const Vec3f& _arm2, uint32_t slices) 
	: GameObject(), axis(_axis), arm1(_arm1), arm2(_arm2), sliceCount(slices) {

}

Cylinder::Cylinder(const Cylinder& other) {
	operator=(other);
}

Cylinder::~Cylinder() {

}

Cylinder& Cylinder::operator=(const Cylinder& other) {
	if (this == &other)
		return *this;

	if (0 < other.pMesh.use_count()) {
		pMesh = other.pMesh;
	}

	return *this;
}

bool Cylinder::Initialize(Renderer& renderer) {
	GameObject::Initialize(renderer);

	//MeshBuilder Call
	MeshBuilder meshBuilder;
	meshBuilder.AddCylinder(axis, arm1, arm2, sliceCount, RGBA::BLUE);

	if (!pMesh)
		pMesh = std::make_shared<Mesh>();

	if (!pMesh)
		return false;

	meshBuilder.CopyToMesh(renderer, pMesh.get(), &Vertex::BindVertexBuffer, &Vertex::Copy, sizeof(Vertex));
	
	albedoMap = TextureLoader::GetTexture(renderer, L"Capture.bmp");

	Vec3f diffuseColor(0.8f, 0.85f, 0.85f);
	Vec4f ambientColor(0.3f, 0.3f, 0.3f, 1.0f);
	Vec3f specularColor(1.0f, 1.0f, 1.0f);
	material = std::make_shared<Material>(diffuseColor, ambientColor, specularColor, std::make_pair(Material::TextureType::TEXTURE_ALBEDO, albedoMap->textureID));
	
	return true;
}


void Cylinder::Shutdown(Renderer& renderer) {
	GameObject::Shutdown(renderer);
	pMesh->Shutdown(renderer);
}

void Cylinder::Update(float deltaTime) {

}

void Cylinder::Render(Renderer& renderer, const ShaderParameter& shaderParam) {
	GameObject::Render(renderer, shaderParam);
	pMesh->Render(renderer);
	boundingVolume->Render(renderer, shaderParam.viewMatrix, shaderParam.projectionMatrix);
}

