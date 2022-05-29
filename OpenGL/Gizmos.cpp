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

Gizmos::GizmoHandle::GizmoHandle(const Gizmos& _owner, std::function<bool(Gizmos::GizmoHandle&, Renderer&)> _meshBuildFunction, const RGBA& _baseColor, const RGBA& _highlightColor, GizmoHandle::eHandle handleType)
	: GameObject(), owner(_owner), baseColor(_baseColor), type(handleType)
{
	meshBuildFunction = _meshBuildFunction;
}

Gizmos::GizmoHandle::GizmoHandle(const Gizmos::GizmoHandle& other) 
	: GameObject(), owner(other.owner)
{
	operator=(other);
}


const Gizmos::GizmoHandle& Gizmos::GizmoHandle::operator=(const Gizmos::GizmoHandle& other) {
	if (&other == this)
		return*this;

	baseColor = other.baseColor;
	type = other.type;

	meshBuildFunction = other.meshBuildFunction;

	return *this;
}



bool Gizmos::GizmoHandle::Initialize(Renderer& renderer) {
	if (!meshBuildFunction) {
		assert(0);
		return false;
	}
	meshBuildFunction(*this, renderer);

	defaultShader = std::make_shared<ColorShader>(this);

	if (!defaultShader) {
		assert(0);
		return false;
	}
	if (!defaultShader->Initialize(renderer)) {
		LogError(L"Could not initialize the Default Shader\n");
		return false;
	}

	AddComponent<BoundingBox>();
	auto boundingBox = GetComponent<BoundingBox>();

	boundingBox->SetCenter(transform->GetPosition());
	boundingBox->SetExtent(GetScale());

	boundingVolume = boundingBox;
	if (!boundingVolume->Init(renderer))
		return false;

	return true;
}
bool Gizmos::GizmoHandle::Intersect(const Ray& ray, double& distance) {
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
void Gizmos::GizmoHandle::Render(Renderer& renderer, ShaderParameter& shaderParam) {
	defaultShader->Render(renderer, shaderParam);

	for (auto& mesh : meshes) {
		mesh->Render(renderer);
	}
}

void Gizmos::GizmoHandle::Shutdown(Renderer& renderer) {
	GameObject::Shutdown(renderer);
	
	for (auto& mesh : meshes) {
		mesh->Shutdown(renderer);
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

Gizmos::GizmoHandle::eHandle Gizmos::GizmoHandles::Intersect(const Ray& ray, double& distance, eTransformMode transformMode) {
	GizmoHandle::eHandle handle = GizmoHandle::eHandle::NONE;
	bool bIntersect = false;
	switch (transformMode) {
	case eTransformMode::TRANSLATE:
	{
		double tempDistance = DBL_MAX;
		bIntersect = at(GizmoHandle::eHandle::TRANSLATE_X).Intersect(ray, tempDistance);
		if (bIntersect) {
			handle = GizmoHandle::eHandle::TRANSLATE_X;
			distance = tempDistance;
		}

		bIntersect = at(GizmoHandle::eHandle::TRANSLATE_Y).Intersect(ray, tempDistance);
		
		if (bIntersect) {
			handle = GizmoHandle::eHandle::TRANSLATE_Y;
			distance = tempDistance;
		}
		
		bIntersect = at(GizmoHandle::eHandle::TRANSLATE_Z).Intersect(ray, tempDistance);
		if (bIntersect) {
			handle = GizmoHandle::eHandle::TRANSLATE_Z;
			distance = tempDistance;
		}

		break;
	}
	case eTransformMode::ROTATE:
	{
		double tempDistance = DBL_MAX;
		bIntersect = at(GizmoHandle::eHandle::ROTATE_X).Intersect(ray, tempDistance);
		if (bIntersect) {
			handle = GizmoHandle::eHandle::ROTATE_X;
			distance = tempDistance;
		}

		bIntersect = at(GizmoHandle::eHandle::ROTATE_Y).Intersect(ray, tempDistance);
		if (bIntersect) {
			handle = GizmoHandle::eHandle::ROTATE_Y;
			distance = tempDistance;
		}

		bIntersect = at(GizmoHandle::eHandle::ROTATE_Z).Intersect(ray, tempDistance);
		if (bIntersect) {
			handle = GizmoHandle::eHandle::ROTATE_Z;
			distance = tempDistance;
		}

		break;
	}
	}

	return handle;
}
void Gizmos::GizmoHandles::Render(Renderer& renderer, ShaderParameter& shaderParam, eTransformMode transformMode) {
	Renderer::DrawMode oldDrawMode = renderer.GetDrawMode();
	renderer.EnableCulling(false);

	switch (transformMode) {
	case eTransformMode::TRANSLATE:
	{
		renderer.SetDrawMode(Renderer::DrawMode::TRIANGLES);
		shaderParam.objNo = (int)GizmoHandle::eHandle::TRANSLATE_X;
		at(GizmoHandle::eHandle::TRANSLATE_X).Render(renderer, shaderParam);

		shaderParam.objNo = (int)GizmoHandle::eHandle::TRANSLATE_Y;
		at(GizmoHandle::eHandle::TRANSLATE_Y).Render(renderer, shaderParam);

		shaderParam.objNo = (int)GizmoHandle::eHandle::TRANSLATE_Z;
		at(GizmoHandle::eHandle::TRANSLATE_Z).Render(renderer, shaderParam);

		break;
	}
	case eTransformMode::ROTATE:
	{
		renderer.SetDrawMode(Renderer::DrawMode::TRIANGLE_STRIP);

		shaderParam.objNo = (int)GizmoHandle::eHandle::ROTATE_X;
		at(GizmoHandle::eHandle::ROTATE_X).Render(renderer, shaderParam);

		shaderParam.objNo = (int)GizmoHandle::eHandle::ROTATE_Y;
		at(GizmoHandle::eHandle::ROTATE_Y).Render(renderer, shaderParam);

		shaderParam.objNo = (int)GizmoHandle::eHandle::ROTATE_Z;
		at(GizmoHandle::eHandle::ROTATE_Z).Render(renderer, shaderParam);

		break;
	}
	case eTransformMode::SCALE:
	{

		break;
	}
	}
	renderer.SetDrawMode(oldDrawMode);
	renderer.EnableCulling(true);
}


Gizmos::Gizmos() 
	:GameObject()
	,transformMode(eTransformMode::ROTATE)
	,oldMousePoint()
{

}

Gizmos::eTransformMode Gizmos::GetMode() const {
	return transformMode;
}


bool Gizmos::Initialize(Renderer& renderer) {
	Gizmos::GizmoHandle translationXHandle {
		Gizmos::GizmoHandle(*this, [&](Gizmos::GizmoHandle& handle, Renderer& renderer) -> bool {
			std::list<std::shared_ptr<Mesh>>& meshes = handle.GetMeshes();
			MeshBuilder meshBuilder;
			meshes.push_back(std::make_shared<Mesh>());
			meshBuilder.Begin();
			meshBuilder.AddCylinder(Vec3f(0.25f, 0.0f, 0.0f), Vec3f(0.0f, 0.01f, 0.0f), Vec3f(0.0f, 0.0f, 0.01f), 32, RGBA::RED);
			meshBuilder.CopyToMesh(renderer, *meshes.back(), &Vertex::BindVertexBuffer, &Vertex::Copy, sizeof(Vertex));
			meshBuilder.End();

			meshes.push_back(std::make_shared<Mesh>());
			meshBuilder.Begin();
			meshBuilder.AddXAxisCone(Vec3f(0.25f, 0.0f, 0.0f), 0.1f, 0.05f, 0.05f, 0.1f, RGBA::RED);
			meshBuilder.CopyToMesh(renderer, *meshes.back(), &Vertex::BindVertexBuffer, &Vertex::Copy, sizeof(Vertex));
			meshBuilder.End();

			return true;
			}, RGBA::RED, RGBA::YELLOW, Gizmos::GizmoHandle::eHandle::TRANSLATE_X)
	};

	handles.Add(translationXHandle);

	Gizmos::GizmoHandle translationYHandle{
		GizmoHandle(*this, [&](Gizmos::GizmoHandle& handle, Renderer& renderer) -> bool {
			std::list<std::shared_ptr<Mesh>>& meshes = handle.GetMeshes();
			MeshBuilder meshBuilder;
			meshes.push_back(std::make_shared<Mesh>());
			meshBuilder.Begin();
			meshBuilder.AddCylinder(Vec3f(0.0f, 0.25f, 0.0f), Vec3f(0.0f, 0.0f, 0.01f), Vec3f(0.01f, 0.0f, 0.0f), 32, RGBA::GREEN);
			meshBuilder.CopyToMesh(renderer, *meshes.back(), &Vertex::BindVertexBuffer, &Vertex::Copy, sizeof(Vertex));
			meshBuilder.End();

			meshes.push_back(std::make_shared<Mesh>());
			meshBuilder.Begin();
			meshBuilder.AddYAxisCone(Vec3f(0.0f, 0.25f, 0.0f), 0.05f, 0.1f, 0.05f, 0.1f, RGBA::GREEN);
			meshBuilder.CopyToMesh(renderer, *meshes.back(), &Vertex::BindVertexBuffer, &Vertex::Copy, sizeof(Vertex));
			meshBuilder.End();
			return true;

			}, RGBA::GREEN, RGBA::YELLOW, Gizmos::GizmoHandle::eHandle::TRANSLATE_Y)
	};

	handles.Add(translationYHandle);


	Gizmos::GizmoHandle translationZHandle {
		GizmoHandle(*this, [&](Gizmos::GizmoHandle& handle, Renderer& renderer) -> bool {
			std::list<std::shared_ptr<Mesh>>& meshes = handle.GetMeshes();
			MeshBuilder meshBuilder;
			meshes.push_back(std::make_shared<Mesh>());
			meshBuilder.Begin();
			meshBuilder.AddCylinder(Vec3f(0.0f, 0.0f, 0.25f), Vec3f(0.01f, 0.0f, 0.0f), Vec3f(0.0f, 0.01f, 0.0f), 32, RGBA::BLUE);
			meshBuilder.CopyToMesh(renderer, *meshes.back(), &Vertex::BindVertexBuffer, &Vertex::Copy, sizeof(Vertex));
			meshBuilder.End();

			meshes.push_back(std::make_shared<Mesh>());
			meshBuilder.Begin();
			meshBuilder.AddZAxisCone(Vec3f(0.0f, 0.0f, 0.25f), 0.05f, 0.05f, 0.1f, 0.1f, RGBA::BLUE);
			meshBuilder.CopyToMesh(renderer, *meshes.back(), &Vertex::BindVertexBuffer, &Vertex::Copy, sizeof(Vertex));
			meshBuilder.End();
			
			return true;
			}, RGBA::BLUE, RGBA::YELLOW, Gizmos::GizmoHandle::eHandle::TRANSLATE_Z)
	};

	handles.Add(translationZHandle);

	Gizmos::GizmoHandle rotationXHandle{
		GizmoHandle(*this, [&](Gizmos::GizmoHandle& handle, Renderer& renderer) -> bool {
			std::list<std::shared_ptr<Mesh>>& meshes = handle.GetMeshes();

			MeshBuilder meshBuilder;
			meshes.push_back(std::make_shared<Mesh>());
			meshBuilder.Begin();
			meshBuilder.AddXAxisCircle(1.0f, 0.95f, RGBA::RED);
			meshBuilder.CopyToMesh(renderer, *meshes.back(), &Vertex::BindVertexBuffer, &Vertex::Copy, sizeof(Vertex), Mesh::TriangleType::MeshType_Strip);
			meshBuilder.End();

			return true;
		}, RGBA::RED, RGBA::YELLOW, Gizmos::GizmoHandle::eHandle::ROTATE_X)
	};

	handles.Add(rotationXHandle);

	Gizmos::GizmoHandle rotationYHandle{
		GizmoHandle(*this, [&](Gizmos::GizmoHandle& handle, Renderer& renderer)->bool {
			std::list<std::shared_ptr<Mesh>>& meshes = handle.GetMeshes();

			MeshBuilder meshBuilder;
			meshes.push_back(std::make_shared<Mesh>());
			meshBuilder.Begin();
			meshBuilder.AddYAxisCircle(1.0f, 0.95f, RGBA::GREEN);
			meshBuilder.CopyToMesh(renderer, *meshes.back(), &Vertex::BindVertexBuffer, &Vertex::Copy, sizeof(Vertex), Mesh::TriangleType::MeshType_Strip);
			meshBuilder.End();

			return true;
		}, RGBA::GREEN, RGBA::YELLOW, Gizmos::GizmoHandle::eHandle::ROTATE_Y)
	};

	handles.Add(rotationYHandle);


	Gizmos::GizmoHandle rotationZHandle{
		GizmoHandle(*this, [&](Gizmos::GizmoHandle& handle, Renderer& renderer)->bool {
			std::list<std::shared_ptr<Mesh>>& meshes = handle.GetMeshes();

			MeshBuilder meshBuilder;
			meshes.push_back(std::make_shared<Mesh>());
			meshBuilder.Begin();
			meshBuilder.AddZAxisCircle(1.0f, 0.95f, RGBA::BLUE);
			meshBuilder.CopyToMesh(renderer, *meshes.back(), &Vertex::BindVertexBuffer, &Vertex::Copy, sizeof(Vertex), Mesh::TriangleType::MeshType_Strip);
			meshBuilder.End();

			return true;

		}, RGBA::BLUE, RGBA::YELLOW, Gizmos::GizmoHandle::eHandle::ROTATE_Z)
	};

	handles.Add(rotationZHandle);

#ifdef _DEBUG
	if (!VerifyGizmos()) {
		assert(0);
	}
#endif 

	for (auto& handle : handles) {
		handle.second.Initialize(renderer);
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
	if (!handles.DoesExist(GizmoHandle::eHandle::ROTATE_X))		return false;
	if (!handles.DoesExist(GizmoHandle::eHandle::ROTATE_Y))		return false;
	if (!handles.DoesExist(GizmoHandle::eHandle::ROTATE_Z))		return false;

	return true;
}
#endif



bool Gizmos::Intersect(const Ray& ray, double& distance) {
	if (targets.empty())
		return false;

	Gizmos::eTransformMode transformMode = GetMode();	
	//기즈모 스케일 처리가 들어가야함
	GizmoHandle::eHandle handleType = GizmoHandle::eHandle::NONE;;
	switch (transformMode) {
	case Gizmos::eTransformMode::TRANSLATE: handleType = handles.Intersect(ray, distance, transformMode);	break;
	case Gizmos::eTransformMode::ROTATE:	handleType = handles.Intersect(ray, distance, transformMode);	break;
	/*case Gizmos::eTransformMode::SCALE:			return IntersectScaleGizmos(ray, distance);*/
	default:
		assert(0);
		break;
	}
	if (handleType != GizmoHandle::eHandle::NONE) {
		Focus(handleType);
	}else {
		context.editingHandle = GizmoHandle::eHandle::NONE;
	}
	return handleType == GizmoHandle::eHandle::NONE ? false : true;
}


void Gizmos::Attach(GameObject& pGameObject) {	
	if(IsAlreadyAttached())
		Detach();
	targets.push_back(&pGameObject);
}

void Gizmos::Detach() {
	targets.clear();
}

void Gizmos::Focus(GizmoHandle::eHandle handleType) {
	if (handleType == GizmoHandle::eHandle::NONE) {
		assert(0);
		return;
	}
	context.editingHandle = handleType;
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
	
	//Calculate Center of Objects 
	Matrix<float, 4, 4> worldMatrix = transform->GetWorldMatrix();
	MakeWorldMatrix(targets[0]->GetPosition(), targets[0]->GetScale(), Vec3f::FORWARD, Vec3f::RIGHT, Vec3f::UP, worldMatrix);
	shaderParam.worldMatrix = worldMatrix;
	transform->SetWorldMatrix(worldMatrix);
	pCamera->GetViewMatrix(shaderParam.viewMatrix);
	shaderParam.projectionMatrix = scene.GetProjectionMatrix();

	shaderParam.focusInfo.focusObjNo = (int)context.editingHandle;
	shaderParam.focusInfo.highlightColor = Vec4f(1.0f, 0.8f, 0.0f, 1.0f);
	
	handles.Render(renderer, shaderParam, GetMode());
}