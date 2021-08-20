#include "BoundingVolume.h"
#include "Renderer.h"
#include "Object.h"
#include "Shader.h"
#include "Logger.h"

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