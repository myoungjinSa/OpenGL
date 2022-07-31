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

Volumef BoundingVolume::CalculateVolume(const Meshes& meshes) {
	std::vector<Volumef> volumes;
	for (size_t iMesh = 0; iMesh < meshes.size(); iMesh++) {
		const std::shared_ptr<Mesh>& mesh = meshes.at(iMesh);
		assert(mesh);
		Volumef volume = mesh->GetVolume();
		volumes.push_back(volume);
	}

	Volumef volume;
	for (const Volumef& vol : volumes) {
		volume |= vol;
	}
	return volume;
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

	return InitVolume(pGameObject->GetMeshes());
}

void BoundingVolume::Render(Renderer& renderer, const Matrix<float, 4, 4>& viewMatrix, const Matrix<float, 4, 4>& projectionMatrix) {

}


BoundingBox::BoundingBox(Object* pOwner)
	:BoundingVolume(pOwner), mesh(nullptr)
{}

bool BoundingBox::InitVolume(const Meshes& meshes) {
	if (meshes.empty())
		return false;

	Volumef volume = CalculateVolume(meshes);
	Vec3f center = volume.GetCenter();
	SetCenter(center);
	Vec3f extent = volume.GetSize();
	SetExtent(extent);

	return true;
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

std::vector<Vec3f> BoundingBox::GetPoints()const {
	std::vector<Vec3f> points;
	points.push_back(Vec3f(center.x - extent.x, center.y - extent.y, center.z - extent.z));	//left-bottom-front
	points.push_back(Vec3f(center.x - extent.x, center.y - extent.y, center.z + extent.z));
	points.push_back(Vec3f(center.x - extent.x, center.y + extent.y, center.z - extent.z));
	points.push_back(Vec3f(center.x - extent.x, center.y + extent.y, center.z + extent.z));
	points.push_back(Vec3f(center.x + extent.x, center.y - extent.y, center.z - extent.z));
	points.push_back(Vec3f(center.x + extent.x, center.y - extent.y, center.z + extent.z));
	points.push_back(Vec3f(center.x + extent.x, center.y + extent.y, center.z - extent.z));
	points.push_back(Vec3f(center.x + extent.x, center.y + extent.y, center.z + extent.z));
	return points;
}

Vec3f BoundingBox::GetNearestPointFromTarget(const GameObject& obj)const {
	Vec3f objPosition = obj.GetPosition();

	std::vector<Vec3f> points = GetPoints();
	assert(!points.empty());

	float minDistance = DBL_MAX;
	for (const auto& pt : points) {
		Vec3f direction = pt - objPosition;
		float distance = sqrt(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);
		if (distance < minDistance)
			minDistance = distance;
	}
	return minDistance;
}

bool BoundingBox::Init(Renderer& renderer) {
	BoundingVolume::Init(renderer);
	
	MeshBuilder meshBuilder;
	meshBuilder.AddCube(center, Vec3f(1.0f, 1.0f, 1.0f), RGBA::GREEN);

	if (!mesh)
		mesh = std::make_shared<Mesh>();

	if (!mesh)
		return false;

	meshBuilder.CopyToMesh(renderer, *mesh, &Vertex::BindVertexBuffer, &Vertex::Copy, sizeof(Vertex));
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

Volumef BoundingBox::GetVolume() {
	Volumef volume;
	GetMinMaxRange(volume);
	return volume;
}

void BoundingBox::GetMinMaxRange(Vec3f& min, Vec3f& max) {
	min.x = center.x - fabs(extent.x);		//right
	min.y = center.y - fabs(extent.y);		//down
	min.z = center.z - fabs(extent.z);		//back

	max.x = center.x + fabs(extent.x);		//left
	max.y = center.y + fabs(extent.y);		//up
	max.z = center.z + fabs(extent.z);		//front
}

void BoundingBox::GetMinMaxRange(Volumef& volume) {
	GetMinMaxRange(volume.min, volume.max);
}

void BoundingBox::Render(Renderer& renderer, const Matrix<float, 4, 4>& viewMatrix, const Matrix<float, 4, 4>& projectionMatrix) {
	Renderer::DrawMode oldDrawMode = renderer.GetDrawMode();
	renderer.SetDrawMode(Renderer::DrawMode::LINES);
	
	ShaderParameter shaderParam;
	Matrix<float, 4, 4> worldViewMatrix = Matrix<float, 4, 4>::Identity();

	Matrix<float, 4, 4> worldMatrix = pGameObject->GetWorldMatrix();
	MakeWorldViewMatrix(worldMatrix, viewMatrix, worldViewMatrix);
	shaderParam.worldViewMatrix = worldViewMatrix;
	shaderParam.projectionMatrix = projectionMatrix;
	
	DefaultShader->Render(renderer, shaderParam);
	mesh->Render(renderer);

	renderer.SetDrawMode(oldDrawMode);
}

BoundingSphere::BoundingSphere(Object* pOwner) 
	: BoundingVolume(pOwner)
{

}

bool BoundingSphere::Init(Renderer& renderer) {

	return true;
}

bool BoundingSphere::InitVolume(const Meshes& meshes) {
	if (meshes.empty())
		return false;

	Volumef volume = CalculateVolume(meshes);
	Vec3f center = volume.GetCenter();
	SetCenter(center);

	float maxValue = volume.GetXSize();
	if (maxValue < volume.GetYSize()) 
		maxValue = volume.GetYSize();
	
	if (maxValue < volume.GetZSize())
		maxValue = volume.GetZSize();

	float radius = maxValue;
	SetRadius(radius);

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

void BoundingSphere::GetMinMaxRange(Vec3f& min, Vec3f& max) {
	min.x = center.x - radius;		//right
	min.y = center.y - radius;		//down
	min.z = center.z - radius;		//back

	max.x = center.x + radius;		//left
	max.y = center.y + radius;		//up
	max.z = center.z + radius;		//front
}

void BoundingSphere::GetMinMaxRange(Volumef& volume) {
	GetMinMaxRange(volume.min, volume.max);
}

Volumef BoundingSphere::GetVolume() {
	Volumef volume;
	GetMinMaxRange(volume);
	return volume;
}