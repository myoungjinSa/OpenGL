#include "Gizmos.h"
#include "Camera.h"
#include "Transform.h"
#include "Renderer.h"
#include "Logger.h"
#include "Shader.h"
#include "Scene.h"
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

			meshBuilder.CopyToMesh(renderer, pGizmoMesh.get(), &ColorVertex::BindVertexBuffer, &ColorVertex::Copy, sizeof(ColorVertex));
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

			meshBuilder.CopyToMesh(renderer, pGizmoMesh.get(), &ColorVertex::BindVertexBuffer, &ColorVertex::Copy, sizeof(ColorVertex));
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

			meshBuilder.CopyToMesh(renderer, pGizmoMesh.get(), &ColorVertex::BindVertexBuffer, &ColorVertex::Copy, sizeof(ColorVertex));
			
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

void Gizmos::GizmoImpl::Update(float deltaTime) {

}
void Gizmos::GizmoImpl::Render(Renderer& renderer, const GizmoParameter& gizmoParam, Camera* pCamera, const Scene& scene) {
	renderer.SetDrawMode(Renderer::DrawMode::TRIANGLES);
	
	ShaderParameter shaderParam;
	Matrix<float, 4, 4> worldMatrix = Matrix<float, 4, 4>::Identity();
	//MakeWorldMatrix(pGameObject->GetPosition(), pGameObject->GetLook(), pGameObject->GetRight(), pGameObject->GetUp(), worldMatrix);
	MakeWorldMatrix(Vec3f(0.0f, 0.0f, 0.0f), -Vec3f::FORWARD, Vec3f::RIGHT, Vec3f::UP, worldMatrix);
	shaderParam.worldMatrix = worldMatrix;
	pCamera->GetViewMatrix(shaderParam.viewMatrix);
	shaderParam.projectionMatrix = scene.GetProjectionMatrix();

	defaultShader->Render(renderer, shaderParam);
	meshComponents[eInteract::TRANSLATE_X].pGizmoMesh->Render(renderer);
	meshComponents[eInteract::TRANSLATE_Y].pGizmoMesh->Render(renderer);
	meshComponents[eInteract::TRANSLATE_Z].pGizmoMesh->Render(renderer);

}

Gizmos::Gizmos() 
	:Object()
{

}

bool Gizmos::Initialize(Renderer& renderer) {
	impl = std::make_unique<GizmoImpl>(this, renderer);
	if (!impl)
		return false;

	return true;
}

void Gizmos::Update(float deltaTime) {
	impl->Update(deltaTime);
}
void Gizmos::Render(Renderer& renderer, const GizmoParameter& parameter, Camera* pCamera, const Scene& scene) {
	impl->Render(renderer, parameter, pCamera, scene);
}