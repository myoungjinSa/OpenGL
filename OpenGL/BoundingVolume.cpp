#include "BoundingVolume.h"
#include "Renderer.h"
#include "Object.h"
#include "Shader.h"
#include "Logger.h"
#include "RayCast.h"

BoundingVolume::BoundingVolume(Object* pObject)
	: Component(pObject), pGameObject(nullptr)
{
	
}


bool BoundingVolume::Init(Renderer& renderer) {
	pGameObject = dynamic_cast<GameObject*>(pOwner);
	if (!pGameObject) {
		LogError(L"Target game object is null.\n");
		return false;
	}

	DefaultShader = std::make_shared<ColorShader>(pGameObject);
	if (!DefaultShader)
		return false;

	if (!DefaultShader->Initialize(renderer)) {
		LogError(L"Could not initialize the Default Shader\n");
		return false;
	}
}

void BoundingVolume::Render(Renderer& renderer, const Matrix<float, 4, 4>& viewMatrix, const Matrix<float, 4, 4>& projectionMatrix) {

}


BoundingBox::BoundingBox(Object* pOwner)
	:BoundingVolume(pOwner), pMesh(nullptr)
{

}

void BoundingBox::SetCenter(const Vec3f& _center) {
	center = _center;
}

void BoundingBox::SetExtent(const Vec3f& _extent) {
	extent = _extent;
}

const Vec3f& BoundingBox::GetCenter() const {
	return center;
}

const Vec3f& BoundingBox::GetExtent() const {
	return extent;
}

bool BoundingBox::Init(Renderer& renderer) {
	BoundingVolume::Init(renderer);

	MeshBuilder meshBuilder;
	meshBuilder.AddCube(center, extent, RGBA::GREEN);

	if (!pMesh)
		pMesh = std::make_shared<Mesh>();

	if (!pMesh)
		return false;

	meshBuilder.CopyToMesh(renderer, pMesh.get(), &ColorVertex::BindVertexBuffer, &ColorVertex::Copy, sizeof(ColorVertex));

}

bool BoundingBox::IsIn(const Vec3f& pos) {




	return true;
}

bool BoundingBox::IsIn(const Ray& ray) {
	return IntersectAABB(ray);
}

bool BoundingBox::IntersectAABB(const Ray& ray) {
	Vec3f min, max;
	GetMinMaxRange(min, max);

	float txMin = (min.x - ray.GetPosition().x) / ray.GetDirection().x;
	float txMax = (max.x - ray.GetPosition().x) / ray.GetDirection().x;

	if (txMax < txMin)
	{
		float temp = 0.0f;
		temp = txMin;
		txMin = txMax;
		txMax = temp;
	}

	float tyMin = (min.y - ray.GetPosition().y) / ray.GetDirection().y;
	float tyMax = (max.y - ray.GetPosition().y) / ray.GetDirection().y;

	if (tyMax < tyMin)
	{
		float temp = 0.0f;
		temp = tyMin;
		tyMin = tyMax;
		tyMax = temp;
	}

	if ((tyMax < txMin) || (txMax < tyMin))
		return false;

	if (txMin < tyMin)
		txMin = tyMin;

	float tzMin = (min.z - ray.GetPosition().z) / ray.GetDirection().z;
	float tzMax = (max.z - ray.GetPosition().z) / ray.GetDirection().z;

	if (tzMax < tzMin) {
		float temp = 0.0f;
		temp = tzMin;
		tzMin = tzMax;
		tzMax = temp;
	}

	if ((tzMax < txMin) || (txMax < tzMin))
		return false;

	if (txMin < tzMin)
		txMin = tzMin;

	if (tzMax < txMax)
		txMax = tzMax;

	return true;
}

void BoundingBox::GetMinMaxRange(Vec3f& min, Vec3f& max) {
	min.x = center.x - fabs(extent.x);		//right
	min.y = center.y - fabs(extent.y);		//down
	min.z = center.z - fabs(extent.z);		//back

	max.x = center.x + fabs(extent.x);		//left
	max.y = center.y + fabs(extent.y);		//up
	max.z = center.z + fabs(extent.z);		//front
}

void BoundingBox::Render(Renderer& renderer, const Matrix<float, 4, 4>& viewMatrix, const Matrix<float, 4, 4>& projectionMatrix) {
	renderer.SetDrawMode(Renderer::DrawMode::LINES);
	
	ShaderParameter shaderParam;
	Matrix<float, 4, 4> worldMatrix = Matrix<float, 4, 4>::Identity();
	MakeWorldMatrix(pGameObject->GetPosition(), pGameObject->GetLook(), pGameObject->GetRight(), pGameObject->GetUp(), worldMatrix);

	shaderParam.worldMatrix = worldMatrix;
	shaderParam.viewMatrix = viewMatrix;
	shaderParam.projectionMatrix = projectionMatrix;

	DefaultShader->Render(renderer, shaderParam);
	pMesh->Render(renderer);
}

BoundingSphere::BoundingSphere(Object* pOwner) 
	: BoundingVolume(pOwner)
{

}

bool BoundingSphere::Init(Renderer& renderer) {

	return true;
}

bool BoundingSphere::IsIn(const Ray& ray) {

	return true;
}
bool BoundingSphere::IsIn(const Vec3f& pos) {

	return true;
}

void BoundingSphere::Render(Renderer& renderer, const Matrix<float, 4, 4>& viewMatrix, const Matrix<float, 4, 4>& projectionMatrix) {
	renderer.SetDrawMode(Renderer::DrawMode::LINES);

}

void BoundingSphere::SetCenter(const Vec3f& _center) {
	center = _center;
}

void BoundingSphere::SetRadius(float _radius) {
	radius = _radius;
}