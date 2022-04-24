#include "Gizmos.h"
#include "Camera.h"
#include "Transform.h"
#include "Renderer.h"
#include "Logger.h"
#include "Shader.h"
#include "Scene.h"
#include "RayCast.h"
#include "Transform.h"
#include "String/String.h"
#include "Input.h"

Gizmos::GizmoHandle::GizmoHandle(const Gizmos& _owner, std::function<std::list<std::shared_ptr<Mesh>>()> meshBuildFunction, const RGBA& _baseColor, const RGBA& _highlightColor, GizmoHandle::eHandle handleType)
	: owner(_owner), baseColor(_baseColor), highlightColor(_highlightColor), type(handleType)
{
	meshes = meshBuildFunction();
}


Gizmos::GizmoHandle::GizmoHandle(const GizmoHandle& other)
	: owner(other.owner)
{
	operator=(other);
}

const Gizmos::GizmoHandle& Gizmos::GizmoHandle::operator=(const Gizmos::GizmoHandle& other) {
	meshes = other.meshes;
	baseColor = other.baseColor;
	highlightColor = other.highlightColor;
	return *this;
}

bool Gizmos::GizmoHandle::Intersect(const Ray& ray, double& distance) const {
	for (const auto& mesh : meshes) {
		for (size_t iTriangle = 0; iTriangle < mesh->GetTriangleMeshCount(); iTriangle++) {
			Triangle triangleMesh = mesh->GetTriangleMesh(iTriangle);
			Vec4f vertex0 = ::Transform(owner.transform->GetWorldMatrix(), Vec4f(triangleMesh.vertices[0].position, 1.0f));
			Vec4f vertex1 = ::Transform(owner.transform->GetWorldMatrix(), Vec4f(triangleMesh.vertices[1].position, 1.0f));
			Vec4f vertex2 = ::Transform(owner.transform->GetWorldMatrix(), Vec4f(triangleMesh.vertices[2].position, 1.0f));

			if (IntersectTriangle(ray, Vec3f(vertex0.x, vertex0.y, vertex0.z), Vec3f(vertex1.x, vertex1.y, vertex1.z), Vec3f(vertex2.x, vertex2.y, vertex2.z), distance)) {
				return true;
			}
		}
	}

	return false;
}
void Gizmos::GizmoHandle::Render(Renderer& renderer) const {
	for (auto& mesh : meshes) {
		mesh->Render(renderer);
	}
}

bool Gizmos::GizmoHandles::Add(const Gizmos::GizmoHandle& handle) {
	const auto& ret = insert(std::pair<GizmoHandle::eHandle, GizmoHandle>(handle.type, handle));
	if (ret.second == false) {
		//Does Already exist;
		return false;
	}

	return true;
}

bool Gizmos::GizmoHandles::DoesExist(GizmoHandle::eHandle handleType)const {
	return count(handleType) != 0 ? true : false;
}

Gizmos::GizmoHandle::eHandle Gizmos::GizmoHandles::Intersect(const Ray& ray, double& distance, eTransformMode transformMode)const {
	GizmoHandle::eHandle handle = GizmoHandle::eHandle::NONE;
	bool bIntersect = false;
	switch (transformMode) {
	case eTransformMode::TRANSLATE:
	{
		double tempDistance = distance;
		bIntersect = at(GizmoHandle::eHandle::TRANSLATE_X).Intersect(ray, tempDistance);
		if (bIntersect) {
			handle = GizmoHandle::eHandle::TRANSLATE_X;
			distance = tempDistance;
		}

		bIntersect = at(GizmoHandle::eHandle::TRANSLATE_Y).Intersect(ray, tempDistance);
		
		if (bIntersect) {
			if (tempDistance < distance)
				handle = GizmoHandle::eHandle::TRANSLATE_Y;
		}
		
		bIntersect = at(GizmoHandle::eHandle::TRANSLATE_Z).Intersect(ray, tempDistance);
		if (bIntersect) {
			if (tempDistance < distance)
				handle = GizmoHandle::eHandle::TRANSLATE_Z;
		}

		break;
	}

	}

	return handle;
}
void Gizmos::GizmoHandles::Render(Renderer& renderer, eTransformMode transformMode) const {
	
	switch (transformMode) {
	case eTransformMode::TRANSLATE:
	{
		at(GizmoHandle::eHandle::TRANSLATE_X).Render(renderer);
		at(GizmoHandle::eHandle::TRANSLATE_Y).Render(renderer);
		at(GizmoHandle::eHandle::TRANSLATE_Z).Render(renderer);
		break;
	}
	case eTransformMode::ROTATE:
	{

		break;
	}
	case eTransformMode::SCALE:
	{

		break;
	}
	}
}


//Gizmos::GizmoImpl::GizmoImpl(Gizmos* _pOwner, Renderer& renderer) 
//	: pOwner(_pOwner), hasClicked(false), hasReleased(false), localToggle(false), dragging(false)
//{
//	std::vector<Point2f> arrowPoints = { {0.25f, 0.0f} , {0.25f, 0.25f}, {1.0f, 0.10f}, {1.2f, 0.0f} };
//	std::vector<Point2f> mace_points = { { 0.25f, 0 }, { 0.25f, 0.05f },{ 1, 0.05f },{ 1, 0.1f },{ 1.25f, 0.1f }, { 1.25f, 0 } };
//	std::vector<Point2f> ring_points = { { +0.025f, 1 },{ -0.025f, 1 },{ -0.025f, 1 },{ -0.025f, 1.1f },{ -0.025f, 1.1f },{ +0.025f, 1.1f },{ +0.025f, 1.1f },{ +0.025f, 1 } };
//	
//	meshComponents[eInteract::TRANSLATE_X] = {
//		GizmoMeshComponent([&]() -> std::list<std::shared_ptr<Mesh>> {
//			std::list<std::shared_ptr<Mesh>> gizmoMeshes;
//			
//			MeshBuilder meshBuilder;
//			gizmoMeshes.push_back(std::make_shared<Mesh>());
//			meshBuilder.Begin();
//			meshBuilder.AddCylinder(Vec3f(0.25f, 0.0f, 0.0f), Vec3f(0.0f, 0.01f, 0.0f), Vec3f(0.0f, 0.0f, 0.01f), 32, RGBA::RED);
//			meshBuilder.CopyToMesh(renderer, *gizmoMeshes.back(), &Vertex::BindVertexBuffer, &Vertex::Copy, sizeof(Vertex));
//			meshBuilder.End();
//
//			gizmoMeshes.push_back(std::make_shared<Mesh>());
//			meshBuilder.Begin();
//			meshBuilder.AddXAxisCone(Vec3f(0.25f, 0.0f, 0.0f), 0.1f, 0.05f, 0.05f, 0.1f, RGBA::RED);
//			meshBuilder.CopyToMesh(renderer, *gizmoMeshes.back(), &Vertex::BindVertexBuffer, &Vertex::Copy, sizeof(Vertex));
//			meshBuilder.End();
//				
//			return gizmoMeshes;
//			}, RGBA::RED, RGBA::YELLOW) 
//	};
//
//	meshComponents[eInteract::TRANSLATE_Y] = {
//		GizmoMeshComponent([&]() -> std::list<std::shared_ptr<Mesh>> {
//			std::list<std::shared_ptr<Mesh>> gizmoMeshes;
//			MeshBuilder meshBuilder;
//			gizmoMeshes.push_back(std::make_shared<Mesh>());
//			meshBuilder.Begin();
//			meshBuilder.AddCylinder(Vec3f(0.0f, 0.25f, 0.0f), Vec3f(0.0f, 0.0f, 0.01f), Vec3f(0.01f, 0.0f, 0.0f), 32, RGBA::GREEN);
//			meshBuilder.CopyToMesh(renderer, *gizmoMeshes.back(), &Vertex::BindVertexBuffer, &Vertex::Copy, sizeof(Vertex));
//			meshBuilder.End();
//			
//			gizmoMeshes.push_back(std::make_shared<Mesh>());
//			meshBuilder.Begin();
//			meshBuilder.AddYAxisCone(Vec3f(0.0f, 0.25f, 0.0f), 0.05f, 0.1f, 0.05f, 0.1f, RGBA::GREEN);
//			meshBuilder.CopyToMesh(renderer, *gizmoMeshes.back(), &Vertex::BindVertexBuffer, &Vertex::Copy, sizeof(Vertex));
//			meshBuilder.End();
//
//
//			return gizmoMeshes;
//			}, RGBA::GREEN, RGBA::YELLOW)
//	};
//
//	meshComponents[eInteract::TRANSLATE_Z] = { 
//		GizmoMeshComponent([&]() ->std::list<std::shared_ptr<Mesh>> {
//			std::list<std::shared_ptr<Mesh>> gizmoMeshes;
//			MeshBuilder meshBuilder;
//			gizmoMeshes.push_back(std::make_shared<Mesh>());
//			meshBuilder.Begin();
//			meshBuilder.AddCylinder(Vec3f(0.0f, 0.0f, 0.25f), Vec3f(0.01f, 0.0f, 0.0f), Vec3f(0.0f, 0.01f, 0.0f), 32, RGBA::BLUE);
//			meshBuilder.CopyToMesh(renderer, *gizmoMeshes.back(), &Vertex::BindVertexBuffer, &Vertex::Copy, sizeof(Vertex));
//			meshBuilder.End();
//
//			gizmoMeshes.push_back(std::make_shared<Mesh>());
//			meshBuilder.Begin();
//			meshBuilder.AddZAxisCone(Vec3f(0.0f, 0.0f, 0.25f), 0.05f, 0.05f, 0.1f, 0.1f, RGBA::BLUE);
//			meshBuilder.CopyToMesh(renderer, *gizmoMeshes.back(), &Vertex::BindVertexBuffer, &Vertex::Copy, sizeof(Vertex));
//			meshBuilder.End();
//			return gizmoMeshes;
//			}, RGBA::BLUE, RGBA::YELLOW)
//	};
//
//	interactionState = {false , false, Vec3f(), Vec4f(), Vec3f(), Vec3f(), Vec3f(), Vec3f(),eInteract::NONE};
//
//	defaultShader = std::make_shared<ColorShader>(pOwner);
//
//	if (!defaultShader) {
//		assert(0);
//		return;
//	}
//	if (!defaultShader->Initialize(renderer)) {
//		LogError(L"Could not initialize the Default Shader\n");
//		return;
//	}
//}
//
//bool Gizmos::GizmoImpl::Intersect(const Ray& ray, double& distance) {
//	if (!pOwner)
//		return false;
//	Gizmos::eTransformMode transformMode = pOwner->GetMode();
//
//	//기즈모 스케일 처리가 들어가야함
//
//
//	switch (transformMode) {
//	case Gizmos::eTransformMode::TRANSLATE: 	return IntersectTranslationGizmos(ray, distance);
//	case Gizmos::eTransformMode::ROTATE:		return IntersectRotationGizmos(ray, distance);
//	case Gizmos::eTransformMode::SCALE:			return IntersectScaleGizmos(ray, distance);
//	default:
//		assert(0);
//		break;
//	}
//
//	return false;
//}
//bool Gizmos::GizmoImpl::IntersectInternal(const Ray& ray, const Vec3f& v0, const Vec3f& v1, const Vec3f& v2, double* hit_t) {
//	double best_t = std::numeric_limits<double>::infinity(), t;
//	int32_t best_tri = -1;
//
//
//	return false;
//}

//bool Gizmos::GizmoImpl::IntersectTranslationGizmos(const Ray& ray, double& distance) {
//	std::list<std::shared_ptr<Mesh>> translationXMeshes = meshComponents[eInteract::TRANSLATE_X].gizmoMeshes;
//	double best_t = std::numeric_limits<double>::infinity(), t;
//	int32_t best_tri = -1;
//	for (auto& translationXMesh : translationXMeshes) {
//		for (uint32_t iTriangle = 0; iTriangle < translationXMesh->GetTriangleMeshCount(); iTriangle++) {
//			Triangle triangleMesh = translationXMesh->GetTriangleMesh(iTriangle);
//			Vec4f vertex0 = ::Transform(pOwner->transform->GetWorldMatrix(), Vec4f(triangleMesh.vertices[0].position, 1.0f));
//			Vec4f vertex1 = ::Transform(pOwner->transform->GetWorldMatrix(), Vec4f(triangleMesh.vertices[1].position, 1.0f));
//			Vec4f vertex2 = ::Transform(pOwner->transform->GetWorldMatrix(), Vec4f(triangleMesh.vertices[2].position, 1.0f));
//			if (pOwner->IntersectTriangle(ray, Vec3f(vertex0.x, vertex0.y, vertex0.z), Vec3f(vertex1.x, vertex1.y, vertex1.z), Vec3f(vertex2.x, vertex2.y, vertex2.z), t) && t < best_t) {
//				best_t = t;
//				best_tri = iTriangle;
//				distance = best_t;
//				interactionState.active = true;
//				interactionState.originalPosition = pOwner->GetPosition();
//				interactionState.interationMode = eInteract::TRANSLATE_X;
//				//interactionState.rayOrigin = ray.GetPosition();
//				interactionState.rayDirection = ray.GetDirection();
//				interactionState.clickOffset = ray.GetPosition() + ray.GetDirection() * distance;
//			}
//		}
//	}
//
//	best_t = std::numeric_limits<double>::infinity();
//	t = -1;
//
//	std::list<std::shared_ptr<Mesh>> translationYMeshes = meshComponents[eInteract::TRANSLATE_Y].gizmoMeshes;
//	for (auto& translationYMesh : translationYMeshes) {
//		for (uint32_t iTriangle = 0; iTriangle < translationYMesh.get()->GetTriangleMeshCount(); iTriangle++) {
//			Triangle triangleMesh = translationYMesh->GetTriangleMesh(iTriangle);
//			Vec4f vertex0 = ::Transform(pOwner->transform->GetWorldMatrix(), Vec4f(triangleMesh.vertices[0].position, 1.0f));
//			Vec4f vertex1 = ::Transform(pOwner->transform->GetWorldMatrix(), Vec4f(triangleMesh.vertices[1].position, 1.0f));
//			Vec4f vertex2 = ::Transform(pOwner->transform->GetWorldMatrix(), Vec4f(triangleMesh.vertices[2].position, 1.0f));
//			if (pOwner->IntersectTriangle(ray, Vec3f(vertex0.x, vertex0.y, vertex0.z), Vec3f(vertex1.x, vertex1.y, vertex1.z), Vec3f(vertex2.x, vertex2.y, vertex2.z), t) && t < best_t) {
//				best_t = t;
//				best_tri = iTriangle;
//				distance = best_t;
//				interactionState.active = true;
//				interactionState.originalPosition = pOwner->GetPosition();
//				interactionState.interationMode = eInteract::TRANSLATE_Y;
//				//interactionState.rayOrigin = ray.GetPosition();
//				interactionState.rayDirection = ray.GetDirection();
//				interactionState.clickOffset = ray.GetPosition() + ray.GetDirection() * distance;
//			}
//		}
//	}
//	best_t = std::numeric_limits<double>::infinity();
//	t = -1;
//	std::list<std::shared_ptr<Mesh>> translationZMeshes = meshComponents[eInteract::TRANSLATE_Z].gizmoMeshes;
//	for (auto& translationZMesh : translationZMeshes) {
//		for (uint32_t iTriangle = 0; iTriangle < translationZMesh.get()->GetTriangleMeshCount(); iTriangle++) {
//			Triangle triangleMesh = translationZMesh->GetTriangleMesh(iTriangle);
//			Vec4f vertex0 = ::Transform(pOwner->transform->GetWorldMatrix(), Vec4f(triangleMesh.vertices[0].position, 1.0f));
//			Vec4f vertex1 = ::Transform(pOwner->transform->GetWorldMatrix(), Vec4f(triangleMesh.vertices[1].position, 1.0f));
//			Vec4f vertex2 = ::Transform(pOwner->transform->GetWorldMatrix(), Vec4f(triangleMesh.vertices[2].position, 1.0f));
//			if (pOwner->IntersectTriangle(ray, Vec3f(vertex0.x, vertex0.y, vertex0.z), Vec3f(vertex1.x, vertex1.y, vertex1.z), Vec3f(vertex2.x, vertex2.y, vertex2.z), t) && t < best_t) {
//				best_t = t;
//				best_tri = iTriangle;
//				distance = best_t;
//				interactionState.active = true;
//				interactionState.originalPosition = pOwner->GetPosition();
//				interactionState.interationMode = eInteract::TRANSLATE_Z;
//				interactionState.rayOrigin = ray.GetPosition();
//				interactionState.rayDirection = ray.GetDirection();
//				interactionState.clickOffset = ray.GetPosition() + ray.GetDirection() * distance;
//			}
//		}
//	}
//	if (best_t == -1) {
//		pOwner->impl->interactionState.active = false;
//		return false;
//	}
//
//	pOwner->impl->interactionState.clickOffset = ray.GetPosition() + ray.GetDirection() * t;
//	return true;
//}


//
//bool Gizmos::GizmoImpl::IntersectRotationGizmos(const Ray& ray, double& distance) {
//	
//	return false;
//}
//
//bool Gizmos::GizmoImpl::IntersectScaleGizmos(const Ray& ray, double& distance) {
//
//	return false;
//}

//
//void Gizmos::GizmoImpl::Update(const Camera& camera, float deltaTime) {
//	if (interactionState.active == false)
//		return;
//
//	if (pOwner->GetMode() == eTransformMode::TRANSLATE) {
//		for (auto& t : pOwner->targets) {
//			Vec3f position = t->GetPosition();
//			//position += interactionState.clickOffset;
//			switch (interactionState.interationMode) {
//			case eInteract::TRANSLATE_X:	Translate(Vec3f::RIGHT, camera.GetPosition(), position);	break;
//			case eInteract::TRANSLATE_Y:	Translate(Vec3f::UP, camera.GetPosition(), position);		break;
//			case eInteract::TRANSLATE_Z:	Translate(Vec3f::FORWARD, camera.GetPosition(), position);	break;
//			default:
//				assert(0);
//				break;
//			}
//			t->SetPosition(position);
//		}
//	}
//
//
//}

//void Gizmos::GizmoImpl::Translate(const Vec3f& axis, const Vec3f& cameraPosition, Vec3f& position) {
//	if (interactionState.active == false)
//		return;
//
//	position += pOwner->impl->interactionState.clickOffset;
//	if (dragging) {
//		Vec3f planeTangent = Cross(axis, position - cameraPosition);
//		Vec3f planeNormal = Cross(axis, planeTangent);
//
//		DragTranslation(planeNormal, position);
//
//		//LogDebug(L"originalPosition = %.5lf, %.5lf, %.5lf\n", interactionState.originalPosition.x, interactionState.originalPosition.y, interactionState.originalPosition.z);
//		position = interactionState.rayOrigin + axis * DotProduct(position - interactionState.rayOrigin, axis);
//	}
//	position -= pOwner->impl->interactionState.clickOffset;
//	//LogDebug(L"position = %.5lf, %.5lf, %.5lf\n", position.x, position.y, position.z);
//}

//void Gizmos::GizmoImpl::DragTranslation(const Vec3f& planeNormal, Vec3f& position) {
//	if (interactionState.active == false)
//		return;
//
//	if (dragging) {
//		Vec3f planePoint = interactionState.rayOrigin;
//		Ray ray(interactionState.rayOrigin, interactionState.rayDirection, 10000.0f);
//
//		float denom = DotProduct(ray.GetDirection(), planeNormal);
//		if (std::abs(denom) == 0)
//			return;
//
//		float t = DotProduct(planePoint - ray.GetPosition(), planeNormal) / denom;
//		if (t < 0)
//			return;
//
//		position = ray.GetPosition() + ray.GetDirection() * t;
//
//
//		LogDebug(L"Position : %.5lf, %.5lf, %.5lf\n", position.x, position.y, position.z);
//		
//		//Do Snap Translation
//		//..
//	}
//	
//}

//void Gizmos::GizmoImpl::Render(Renderer& renderer, const GizmoParameter& gizmoParam, Camera* pCamera, const Scene& scene) {
//	ShaderParameter shaderParam;
//	const std::vector<GameObject*> targets = gizmoParam.attachTargets;
//	if (targets.empty())
//		return;
//
//	renderer.SetDrawMode(Renderer::DrawMode::TRIANGLES);
//	
//	
//	//Calculate Center of Objects 
//
//	Matrix<float, 4, 4> worldMatrix = pOwner->transform->GetWorldMatrix();
//	MakeWorldMatrix(targets[0]->GetPosition(), targets[0]->GetScale(), Vec3f::FORWARD, Vec3f::RIGHT, Vec3f::UP, worldMatrix);
//	shaderParam.worldMatrix = worldMatrix;
//	pOwner->transform->SetWorldMatrix(worldMatrix);
//	pCamera->GetViewMatrix(shaderParam.viewMatrix);
//	shaderParam.projectionMatrix = scene.GetProjectionMatrix();
//
//	defaultShader->Render(renderer, shaderParam);
//	
//	for (auto& gizmoMesh : meshComponents[eInteract::TRANSLATE_X].gizmoMeshes)
//		gizmoMesh->Render(renderer);
//	for (auto& gizmoMesh : meshComponents[eInteract::TRANSLATE_Y].gizmoMeshes)
//		gizmoMesh->Render(renderer);
//	for (auto& gizmoMesh : meshComponents[eInteract::TRANSLATE_Z].gizmoMeshes)
//		gizmoMesh->Render(renderer);	
//}

Gizmos::Gizmos() 
	:GameObject()
	,transformMode(eTransformMode::TRANSLATE)
	,oldMousePoint()
{

}

Gizmos::eTransformMode Gizmos::GetMode() const {
	return transformMode;
}

Gizmos::GizmoHandle::eHandle Gizmos::GetEditingHandle() const {
	return context.editingHandle;
}

bool Gizmos::Initialize(Renderer& renderer) {
	Gizmos::GizmoHandle translationXHandle {
		Gizmos::GizmoHandle(*this, [&]() -> std::list<std::shared_ptr<Mesh>> {
			std::list<std::shared_ptr<Mesh>> gizmoMeshes;

			MeshBuilder meshBuilder;
			gizmoMeshes.push_back(std::make_shared<Mesh>());
			meshBuilder.Begin();
			meshBuilder.AddCylinder(Vec3f(0.25f, 0.0f, 0.0f), Vec3f(0.0f, 0.01f, 0.0f), Vec3f(0.0f, 0.0f, 0.01f), 32, RGBA::RED);
			meshBuilder.CopyToMesh(renderer, *gizmoMeshes.back(), &Vertex::BindVertexBuffer, &Vertex::Copy, sizeof(Vertex));
			meshBuilder.End();

			gizmoMeshes.push_back(std::make_shared<Mesh>());
			meshBuilder.Begin();
			meshBuilder.AddXAxisCone(Vec3f(0.25f, 0.0f, 0.0f), 0.1f, 0.05f, 0.05f, 0.1f, RGBA::RED);
			meshBuilder.CopyToMesh(renderer, *gizmoMeshes.back(), &Vertex::BindVertexBuffer, &Vertex::Copy, sizeof(Vertex));
			meshBuilder.End();

			return gizmoMeshes;
			}, RGBA::RED, RGBA::YELLOW, Gizmos::GizmoHandle::eHandle::TRANSLATE_X)
	};

	handles.Add(translationXHandle);

	Gizmos::GizmoHandle translationYHandle{
		GizmoHandle(*this, [&]() -> std::list<std::shared_ptr<Mesh>> {
			std::list<std::shared_ptr<Mesh>> gizmoMeshes;
			MeshBuilder meshBuilder;
			gizmoMeshes.push_back(std::make_shared<Mesh>());
			meshBuilder.Begin();
			meshBuilder.AddCylinder(Vec3f(0.0f, 0.25f, 0.0f), Vec3f(0.0f, 0.0f, 0.01f), Vec3f(0.01f, 0.0f, 0.0f), 32, RGBA::GREEN);
			meshBuilder.CopyToMesh(renderer, *gizmoMeshes.back(), &Vertex::BindVertexBuffer, &Vertex::Copy, sizeof(Vertex));
			meshBuilder.End();

			gizmoMeshes.push_back(std::make_shared<Mesh>());
			meshBuilder.Begin();
			meshBuilder.AddYAxisCone(Vec3f(0.0f, 0.25f, 0.0f), 0.05f, 0.1f, 0.05f, 0.1f, RGBA::GREEN);
			meshBuilder.CopyToMesh(renderer, *gizmoMeshes.back(), &Vertex::BindVertexBuffer, &Vertex::Copy, sizeof(Vertex));
			meshBuilder.End();


			return gizmoMeshes;
			}, RGBA::GREEN, RGBA::YELLOW, Gizmos::GizmoHandle::eHandle::TRANSLATE_Y)
	};

	handles.Add(translationYHandle);


	Gizmos::GizmoHandle translationZHandle {
		GizmoHandle(*this, [&]() ->std::list<std::shared_ptr<Mesh>> {
			std::list<std::shared_ptr<Mesh>> gizmoMeshes;
			MeshBuilder meshBuilder;
			gizmoMeshes.push_back(std::make_shared<Mesh>());
			meshBuilder.Begin();
			meshBuilder.AddCylinder(Vec3f(0.0f, 0.0f, 0.25f), Vec3f(0.01f, 0.0f, 0.0f), Vec3f(0.0f, 0.01f, 0.0f), 32, RGBA::BLUE);
			meshBuilder.CopyToMesh(renderer, *gizmoMeshes.back(), &Vertex::BindVertexBuffer, &Vertex::Copy, sizeof(Vertex));
			meshBuilder.End();

			gizmoMeshes.push_back(std::make_shared<Mesh>());
			meshBuilder.Begin();
			meshBuilder.AddZAxisCone(Vec3f(0.0f, 0.0f, 0.25f), 0.05f, 0.05f, 0.1f, 0.1f, RGBA::BLUE);
			meshBuilder.CopyToMesh(renderer, *gizmoMeshes.back(), &Vertex::BindVertexBuffer, &Vertex::Copy, sizeof(Vertex));
			meshBuilder.End();
			return gizmoMeshes;
			}, RGBA::BLUE, RGBA::YELLOW, Gizmos::GizmoHandle::eHandle::TRANSLATE_Z)
	};

	handles.Add(translationZHandle);

#ifdef _DEBUG
	if (!VerifyGizmos()) {
		assert(0);
	}
#endif 

	defaultShader = std::make_shared<ColorShader>(this);

	if (!defaultShader) {
		assert(0);
		return false;
	}
	if (!defaultShader->Initialize(renderer)) {
		LogError(L"Could not initialize the Default Shader\n");
		return false;
	}
	return true;
}


bool Gizmos::IsAlreadyAttached() const {
	return !(targets.empty());
}

const GameObject& Gizmos::GetAttachedObjects(uint32_t index) const {
	assert(index < targets.size());
	return *(targets.at(index));
}

float Gizmos::ScaleByDistanceToTarget(const Vec3f& targetPos, float yfov, float pixelScale) const {
	float dist = ::Length(targetPos - transform.get()->GetPosition());
	return std::tan(yfov) * dist;
}

#ifdef _DEBUG
bool Gizmos::VerifyGizmos() const{
	if (!handles.DoesExist(GizmoHandle::eHandle::TRANSLATE_X))	return false;
	if (!handles.DoesExist(GizmoHandle::eHandle::TRANSLATE_Y))	return false;
	if (!handles.DoesExist(GizmoHandle::eHandle::TRANSLATE_Z))	return false;

	return true;
}
#endif
bool Gizmos::Intersect(const Ray& ray, double& distance) {
	if (targets.empty())
		return false;

	Gizmos::eTransformMode transformMode = GetMode();	
	//기즈모 스케일 처리가 들어가야함
			
	switch (transformMode) {
	case Gizmos::eTransformMode::TRANSLATE:
	{
		context.editingHandle = handles.Intersect(ray, distance, transformMode);
	}
	break;
	/*case Gizmos::eTransformMode::ROTATE:		return IntersectRotationGizmos(ray, distance);
	case Gizmos::eTransformMode::SCALE:			return IntersectScaleGizmos(ray, distance);*/
	default:
		assert(0);
		break;
	}
	
	return context.editingHandle == GizmoHandle::eHandle::NONE ? false : true;
}

//void Gizmos::ProcessEvent(Event& e) {
//	if (MouseInput::MouseEvent* pMouseEvent = dynamic_cast<MouseInput::MouseEvent*>(&e)) {
//		if (pMouseEvent->mouseState == MouseInput::MouseEvent::MOUSE_STATE::MOUSE_STATE_COUNT)
//			return;
//
//		Point2i newMousePoint = pMouseEvent->mousePoint;
//		if (pMouseEvent->mouseState == MouseInput::MouseEvent::MOUSE_STATE::LBUTTON_DOWN) {
//			impl->hasClicked = true;
//			impl->interactionState.interationMode = GizmoImpl::eInteract::NONE;
//		}
//		if (pMouseEvent->mouseState == MouseInput::MouseEvent::MOUSE_STATE::LBUTTON_UP) {
//			impl->dragging = false;
//			LogDebug(L"Gizmos:: Mouse Released\n");
//		}
//
//
//		if (pMouseEvent->mouseState == MouseInput::MouseEvent::MOUSE_STATE::LEFT_BUTTON_DRAG) {
//			Vec2f mouseDelta(newMousePoint.x - oldMousePoint.x, newMousePoint.y - oldMousePoint.y);
//			if (impl->interactionState.active) {
//				impl->dragging = true;
//				impl->hasClicked = false;
//			}
//			if (sqrtf(newMousePoint.x - oldMousePoint.x + newMousePoint.y - oldMousePoint.y) > 50.0f)
//			{
//				oldMousePoint = newMousePoint;
//				return;
//			}
//			
//		}
//		oldMousePoint = newMousePoint;
//	}
//
//}


void Gizmos::Attach(GameObject& pGameObject) {	
	if(IsAlreadyAttached())
		Detach();
	targets.push_back(&pGameObject);
	
	//MouseInput::Attach(this);
}

void Gizmos::Detach() {
	//MouseInput::Detach(this);

	targets.clear();
}


void Gizmos::Render(Renderer& renderer, Camera* pCamera, const Scene& scene) {
	if (targets.empty())
		return;
	
	GizmoParameter parameter;
	parameter.attachTargets.reserve(targets.size());
	for (const auto& t : targets) {
		parameter.attachTargets.push_back(t);
	}
	
	ShaderParameter shaderParam;
	const std::vector<GameObject*> targets = parameter.attachTargets;
	if (targets.empty())
		return;
	
	renderer.SetDrawMode(Renderer::DrawMode::TRIANGLES);
	
	
	//Calculate Center of Objects 
	
	Matrix<float, 4, 4> worldMatrix = transform->GetWorldMatrix();
	MakeWorldMatrix(targets[0]->GetPosition(), targets[0]->GetScale(), Vec3f::FORWARD, Vec3f::RIGHT, Vec3f::UP, worldMatrix);
	shaderParam.worldMatrix = worldMatrix;
	transform->SetWorldMatrix(worldMatrix);
	pCamera->GetViewMatrix(shaderParam.viewMatrix);
	shaderParam.projectionMatrix = scene.GetProjectionMatrix();
	
	defaultShader->Render(renderer, shaderParam);
	
	handles.Render(renderer, GetMode());
}