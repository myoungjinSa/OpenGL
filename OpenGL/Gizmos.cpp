#include "Gizmos.h"
#include "Camera.h"
#include "Transform.h"
#include "Renderer.h"
#include "Logger.h"
#include "Shader.h"
#include "Scene.h"
#include "RayCast.h"
#include "String/String.h"

Gizmos::GizmoImpl::GizmoMeshComponent::GizmoMeshComponent(std::function<std::shared_ptr<Mesh>()> meshBuildFunction, const RGBA& _baseColor, const RGBA& _highlightColor) 
	: baseColor(_baseColor), highlightColor(_highlightColor)
{
	pGizmoMesh = meshBuildFunction();
}


Gizmos::GizmoImpl::GizmoMeshComponent::GizmoMeshComponent(const GizmoMeshComponent& other) {
	operator=(other);
}

const Gizmos::GizmoImpl::GizmoMeshComponent& Gizmos::GizmoImpl::GizmoMeshComponent::operator=(const Gizmos::GizmoImpl::GizmoMeshComponent& other) {
	pGizmoMesh = other.pGizmoMesh;
	baseColor = other.baseColor;
	highlightColor = other.highlightColor;
	return *this;
}

Gizmos::GizmoImpl::GizmoImpl(Gizmos* _pOwner, Renderer& renderer) : pOwner(_pOwner) {
	std::vector<Point2f> arrowPoints = { {0.25f, 0.0f} , {0.25f, 0.25f}, {1.0f, 0.10f}, {1.2f, 0.0f} };
	std::vector<Point2f> mace_points = { { 0.25f, 0 }, { 0.25f, 0.05f },{ 1, 0.05f },{ 1, 0.1f },{ 1.25f, 0.1f }, { 1.25f, 0 } };
	std::vector<Point2f> ring_points = { { +0.025f, 1 },{ -0.025f, 1 },{ -0.025f, 1 },{ -0.025f, 1.1f },{ -0.025f, 1.1f },{ +0.025f, 1.1f },{ +0.025f, 1.1f },{ +0.025f, 1 } };
	
	meshComponents[eInteract::TRANSLATE_X] = {
		GizmoMeshComponent([&]() -> std::shared_ptr<Mesh> {
			MeshBuilder meshBuilder;
			meshBuilder.AddLathGeometry(Vec3f(1.0f, 0.0f, 0.0f), Vec3f(0.0f, 1.0f, 0.0f), Vec3f(0.0f, 0.0f, 1.0f), 16, arrowPoints, RGBA::RED);
			std::shared_ptr<Mesh> pGizmoMesh;
			if (!pGizmoMesh)
				pGizmoMesh = std::make_shared<Mesh>();

			meshBuilder.CopyToMesh(renderer, pGizmoMesh.get(), &Vertex::BindVertexBuffer, &Vertex::Copy, sizeof(Vertex));
			return pGizmoMesh;
			}, RGBA::RED, RGBA::YELLOW) 
	};

	meshComponents[eInteract::TRANSLATE_Y] = {
		GizmoMeshComponent([&]() -> std::shared_ptr<Mesh> {
			MeshBuilder meshBuilder;
			meshBuilder.AddLathGeometry(Vec3f(0.0f, 1.0f, 0.0f), Vec3f(0.0f, 0.0f, 1.0f), Vec3f(1.0f, 0.0f, 0.0f), 16, arrowPoints, RGBA::GREEN);
			std::shared_ptr<Mesh> pGizmoMesh;
			if (!pGizmoMesh)
				pGizmoMesh = std::make_shared<Mesh>();

			meshBuilder.CopyToMesh(renderer, pGizmoMesh.get(), &Vertex::BindVertexBuffer, &Vertex::Copy, sizeof(Vertex));
			return pGizmoMesh;
			}, RGBA::GREEN, RGBA::YELLOW)
	};

	meshComponents[eInteract::TRANSLATE_Z] = { 
		GizmoMeshComponent([&]() -> std::shared_ptr<Mesh> {
			MeshBuilder meshBuilder;
			meshBuilder.AddLathGeometry(Vec3f(0.0f, 0.0f, 1.0f), Vec3f(1.0f, 0.0f, 0.0f), Vec3f(0.0f, 1.0f, 0.0f), 16, arrowPoints, RGBA::BLUE);
			std::shared_ptr<Mesh> pGizmoMesh;
			if (!pGizmoMesh)
				pGizmoMesh = std::make_shared<Mesh>();

			meshBuilder.CopyToMesh(renderer, pGizmoMesh.get(), &Vertex::BindVertexBuffer, &Vertex::Copy, sizeof(Vertex));
			
			return pGizmoMesh;
			}, RGBA::BLUE, RGBA::YELLOW)
	};
	defaultShader = std::make_shared<ColorShader>(pOwner);

	if (!defaultShader) {
		assert(0);
		return;
	}
	if (!defaultShader->Initialize(renderer)) {
		LogError(L"Could not initialize the Default Shader\n");
		return;
	}
}

bool Gizmos::GizmoImpl::Intersect(const Ray& ray, double& distance) {
	Gizmos::eTransformMode transformMode = pOwner->GetMode();

	switch (transformMode) {
	case Gizmos::eTransformMode::TRANSLATE: 	return IntersectTranslationGizmos(ray, distance);
	case Gizmos::eTransformMode::ROTATE:		return IntersectRotationGizmos(ray, distance);
	case Gizmos::eTransformMode::SCALE:			return IntersectScaleGizmos(ray, distance);
	default:
		assert(0);
		break;
	}

	return false;
}

bool Gizmos::GizmoImpl::IntersectTranslationGizmos(const Ray& ray, double& distance) {
	double d = 0.0;
	std::shared_ptr<Mesh> translationXMesh = meshComponents[eInteract::TRANSLATE_X].pGizmoMesh;
	for (uint32_t iTriangle = 0; iTriangle < translationXMesh.get()->GetTriangleMeshCount(); iTriangle++) {
		Triangle triangleMesh = translationXMesh->GetTriangleMesh(iTriangle);
		if (pOwner->IntersectTriangle(ray, triangleMesh.vertices[0].position, triangleMesh.vertices[1].position, triangleMesh.vertices[2].position, false, distance)) {
			return true;
		}

	}

	std::shared_ptr<Mesh> translationYMesh = meshComponents[eInteract::TRANSLATE_Y].pGizmoMesh;
	for (uint32_t iTriangle = 0; iTriangle < translationYMesh.get()->GetTriangleMeshCount(); iTriangle++) {
		Triangle triangleMesh = translationYMesh->GetTriangleMesh(iTriangle);
		if (pOwner->IntersectTriangle(ray, triangleMesh.vertices[0].position, triangleMesh.vertices[1].position, triangleMesh.vertices[2].position, false, distance)) {
			return true;
		}
	}

	std::shared_ptr<Mesh> translationZMesh = meshComponents[eInteract::TRANSLATE_Z].pGizmoMesh;
	for (uint32_t iTriangle = 0; iTriangle < translationZMesh.get()->GetTriangleMeshCount(); iTriangle++) {
		Triangle triangleMesh = translationZMesh->GetTriangleMesh(iTriangle);
		if (pOwner->IntersectTriangle(ray, triangleMesh.vertices[0].position, triangleMesh.vertices[1].position, triangleMesh.vertices[2].position, false, distance)) {
			return true;
		}
	}


	return true;
}

bool Gizmos::GizmoImpl::IntersectRotationGizmos(const Ray& ray, double& distance) {

	return true;
}

bool Gizmos::GizmoImpl::IntersectScaleGizmos(const Ray& ray, double& distance) {

	return true;
}


void Gizmos::GizmoImpl::Update(float deltaTime) {

}
void Gizmos::GizmoImpl::Render(Renderer& renderer, const GizmoParameter& gizmoParam, Camera* pCamera, const Scene& scene) {
	ShaderParameter shaderParam;
	const std::vector<GameObject*> targets = gizmoParam.attachTargets;
	if (targets.empty())
		return;

	renderer.SetDrawMode(Renderer::DrawMode::TRIANGLES);
	
	
	//Calculate Center of Objects 

	
	Matrix<float, 4, 4> worldMatrix = Matrix<float, 4, 4>::Identity();
	MakeWorldMatrix(targets[0]->GetPosition(), targets[0]->GetLook(), targets[0]->GetRight(), targets[0]->GetUp(), worldMatrix);
	shaderParam.worldMatrix = worldMatrix;
	pCamera->GetViewMatrix(shaderParam.viewMatrix);
	shaderParam.projectionMatrix = scene.GetProjectionMatrix();

	defaultShader->Render(renderer, shaderParam);
	meshComponents[eInteract::TRANSLATE_X].pGizmoMesh->Render(renderer);
	meshComponents[eInteract::TRANSLATE_Y].pGizmoMesh->Render(renderer);
	meshComponents[eInteract::TRANSLATE_Z].pGizmoMesh->Render(renderer);

}

Gizmos::Gizmos() 
	:GameObject(), transformMode(eTransformMode::TRANSLATE)
{

}

Gizmos::eTransformMode Gizmos::GetMode() const {
	return transformMode;
}
bool Gizmos::Initialize(Renderer& renderer) {
	impl = std::make_unique<GizmoImpl>(this, renderer);
	if (!impl)
		return false;

	return true;
}

bool Gizmos::IsAlreadyAttached() const {
	return !(targets.empty());
}

const GameObject& Gizmos::GetAttachedObjects(uint32_t index) const {
	assert(index < targets.size());
	return *(targets.at(index));
}

bool Gizmos::Intersect(const Ray& ray, double& distance) {
	if (targets.empty())
		return false;

	impl->Intersect(ray, distance);

	return true;
}

void Gizmos::Attach(GameObject& pGameObject) {	
	if(IsAlreadyAttached())
		Detach();

	targets.push_back(&pGameObject);
}

void Gizmos::Detach() {
	targets.clear();
}

void Gizmos::Update(float deltaTime) {
	if (targets.empty())
		return;

	impl->Update(deltaTime);
}

void Gizmos::Render(Renderer& renderer, Camera* pCamera, const Scene& scene) {
	if (targets.empty())
		return;

	GizmoParameter parameter;
	parameter.attachTargets.reserve(targets.size());
	for (const auto& t : targets) {
		parameter.attachTargets.push_back(t);
	}
	impl->Render(renderer, parameter, pCamera, scene);
}