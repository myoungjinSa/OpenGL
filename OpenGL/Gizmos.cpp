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
#include "Stack.h"

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
	if (!meshBuildFunction(*this, renderer))
		return false;

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
	
	boundingVolume = boundingBox;
	if (!boundingVolume->Init(renderer))
		return false;

	return true;
}

double Gizmos::GizmoHandle::CalcDistanceFromCamera(const Camera& camera) const{
	Vec3f cameraPosition = camera.GetPosition();
	Vec3f position = GetPosition();
	Vec3d direction = cameraPosition - position;
	double distance = sqrt(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);
	return distance;
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

bool Gizmos::GizmoHandles::Intersect(Gizmos::GizmoHandle::eHandle gizmoHandle, const Ray& ray, double& distance) {
	if (Gizmos::GizmoHandle::eHandle::COUNT <= gizmoHandle) {
		return false;
	}

	bool bIntersect = at(gizmoHandle).Intersect(ray, distance);
	return bIntersect;
}


Gizmos::GizmoHandle::eHandle Gizmos::GizmoHandles::GetNearestHandle(std::vector<Gizmos::GizmoHandle::eHandle> handleTypes, const Ray& ray, double distance) {
	GizmoHandle::eHandle handle = GizmoHandle::eHandle::NONE;
	double oldDistance = distance;
	double newDistance = 0.0;

	for (auto handleType : handleTypes) {
		if (Intersect(handleType, ray, newDistance)) {
			if (newDistance < oldDistance) {
				handle = handleType;
				oldDistance = newDistance;
			}
		}
	}
	return handle;
}

Gizmos::GizmoHandle::eHandle Gizmos::GizmoHandles::GetNearestHandleFromCamera(const std::vector<Gizmos::GizmoHandle*>& handles, const Camera& camera) const{
	double oldDistance = DBL_MAX;
	double newDistance = 0.0;

	Gizmos::GizmoHandle::eHandle nearestHandleType;
	for (const auto& handle : handles) {
		newDistance = CalcDistanceFromCamera(*handle, camera);
		if (newDistance < oldDistance) {
			nearestHandleType = handle->type;
			oldDistance = newDistance;
		}
	}
	return nearestHandleType;
}

Gizmos::GizmoHandle::eHandle Gizmos::GizmoHandles::Intersect(const Ray& ray, double& distance, eTransformMode transformMode) {
	GizmoHandle::eHandle handle = GizmoHandle::eHandle::NONE;
	bool bIntersect = false;
	std::vector<Gizmos::GizmoHandle::eHandle> handleTypes;
	switch (transformMode) {
	case eTransformMode::TRANSLATE:
	{		
		handleTypes.push_back(GizmoHandle::eHandle::TRANSLATE_X);
		handleTypes.push_back(GizmoHandle::eHandle::TRANSLATE_Y);
		handleTypes.push_back(GizmoHandle::eHandle::TRANSLATE_Z);
		handleTypes.push_back(GizmoHandle::eHandle::TRANSLATE_XY);
		handleTypes.push_back(GizmoHandle::eHandle::TRANSLATE_XZ);
		handleTypes.push_back(GizmoHandle::eHandle::TRANSLATE_YZ);
		handle = GetNearestHandle(handleTypes, ray, distance);

		break;
	}
	case eTransformMode::ROTATE:
	{
		handleTypes.push_back(GizmoHandle::eHandle::ROTATE_X);
		handleTypes.push_back(GizmoHandle::eHandle::ROTATE_Y);
		handleTypes.push_back(GizmoHandle::eHandle::ROTATE_Z);
		
		handle = GetNearestHandle(handleTypes, ray, distance);
		break;
	}
	case eTransformMode::SCALE:
	{
		handleTypes.push_back(GizmoHandle::eHandle::SCALE_X);
		handleTypes.push_back(GizmoHandle::eHandle::SCALE_Y);
		handleTypes.push_back(GizmoHandle::eHandle::SCALE_Z);

		handle = GetNearestHandle(handleTypes, ray, distance);
		break;
	}
	}

	return handle;
}

double Gizmos::GizmoHandles::CalcDistanceFromCamera(const GizmoHandle& gizmoHandle, const Camera& camera)const {
	Volumef gizmoVolume;
	gizmoHandle.GetLocalBoundingBox(gizmoVolume);
	
	Vec3f gizmoPos = gizmoHandle.owner.GetPosition();
	gizmoVolume.Move(gizmoPos);

	Vec3f cameraPos = camera.GetPosition();
	Vec3d direction = gizmoVolume.GetCenter() - cameraPos;
	double distance = sqrt(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);
	return distance;
}

void Gizmos::GizmoHandles::Render(Renderer& renderer, ShaderParameter& shaderParam, const Camera& camera, eTransformMode transformMode) {
	Renderer::DrawMode oldDrawMode = renderer.GetDrawMode();
	std::vector<GizmoHandle*> handles;
	switch (transformMode) {
	case eTransformMode::TRANSLATE:
	{
		renderer.SetDrawMode(Renderer::DrawMode::TRIANGLES);

		handles.push_back(&at(GizmoHandle::eHandle::TRANSLATE_X));
		handles.push_back(&at(GizmoHandle::eHandle::TRANSLATE_Y));
		handles.push_back(&at(GizmoHandle::eHandle::TRANSLATE_Z));
		handles.push_back(&at(GizmoHandle::eHandle::TRANSLATE_XY));
		handles.push_back(&at(GizmoHandle::eHandle::TRANSLATE_XZ));
		handles.push_back(&at(GizmoHandle::eHandle::TRANSLATE_YZ));

		break;
	}
	case eTransformMode::ROTATE:
	{
		renderer.SetDrawMode(Renderer::DrawMode::TRIANGLE_STRIP);
		handles.push_back(&at(GizmoHandle::eHandle::ROTATE_X));
		handles.push_back(&at(GizmoHandle::eHandle::ROTATE_Y));
		handles.push_back(&at(GizmoHandle::eHandle::ROTATE_Z));

		break;
	}
	case eTransformMode::SCALE:
	{
		renderer.SetDrawMode(Renderer::DrawMode::TRIANGLES);

		handles.push_back(&at(GizmoHandle::eHandle::SCALE_X));
		handles.push_back(&at(GizmoHandle::eHandle::SCALE_Y));
		handles.push_back(&at(GizmoHandle::eHandle::SCALE_Z));

		break;
	}
	}

	Stack<GizmoHandle::eHandle> orderByDistanceFromCamera;
	while (!handles.empty()) {
		GizmoHandle::eHandle nearestHandleType = GetNearestHandleFromCamera(handles, camera);
		handles.erase(std::remove_if(handles.begin(), handles.end(), [=](const GizmoHandle* pHandle) {
			return pHandle->type == nearestHandleType;
			}), handles.end());

		orderByDistanceFromCamera.Push(nearestHandleType);
	}

	while (!orderByDistanceFromCamera.Empty()) {
		GizmoHandle::eHandle handleType = orderByDistanceFromCamera.Peek();
		shaderParam.objNo = (int)handleType;
		at(handleType).Render(renderer, shaderParam);
		orderByDistanceFromCamera.Pop();
	}

	renderer.SetDrawMode(oldDrawMode);
}


Gizmos::Gizmos() 
	:GameObject()
	,transformMode(eTransformMode::TRANSLATE)
	,oldMousePoint()
{

}

Gizmos::eTransformMode Gizmos::GetMode() const {
	return transformMode;
}


bool Gizmos::Initialize(Renderer& renderer) {
	Gizmos::GizmoHandle translationXHandle {
		Gizmos::GizmoHandle(*this, [&](Gizmos::GizmoHandle& handle, Renderer& renderer) -> bool {
			Meshes& meshes = handle.GetMeshes();
			MeshBuilder meshBuilder;
			meshes.Add(std::make_shared<Mesh>());
			meshBuilder.Begin();
			meshBuilder.AddCylinder(Vec3f(orgSize, 0.0f, 0.0f), Vec3f(0.0f, 0.01f, 0.0f), Vec3f(0.0f, 0.0f, 0.01f), 32, RGBA::RED);
			meshBuilder.CopyToMesh(renderer, *meshes.back(), &Vertex::BindVertexBuffer, &Vertex::Copy, sizeof(Vertex));
			meshBuilder.End();

			meshes.Add(std::make_shared<Mesh>());
			meshBuilder.Begin();
			meshBuilder.AddXAxisCone(Vec3f(orgSize, 0.0f, 0.0f), 0.1f, 0.05f, 0.05f, 0.1f, RGBA::RED);
			meshBuilder.CopyToMesh(renderer, *meshes.back(), &Vertex::BindVertexBuffer, &Vertex::Copy, sizeof(Vertex));
			meshBuilder.End();

			return true;
			}, RGBA::RED, RGBA::YELLOW, Gizmos::GizmoHandle::eHandle::TRANSLATE_X)
	};

	if (!handles.Add(translationXHandle))
		assert(0);

	Gizmos::GizmoHandle translationYHandle{
		GizmoHandle(*this, [&](Gizmos::GizmoHandle& handle, Renderer& renderer) -> bool {
			Meshes& meshes = handle.GetMeshes();
			MeshBuilder meshBuilder;
			meshes.Add(std::make_shared<Mesh>());
			meshBuilder.Begin();
			meshBuilder.AddCylinder(Vec3f(0.0f, orgSize, 0.0f), Vec3f(0.0f, 0.0f, 0.01f), Vec3f(0.01f, 0.0f, 0.0f), 32, RGBA::GREEN);
			meshBuilder.CopyToMesh(renderer, *meshes.back(), &Vertex::BindVertexBuffer, &Vertex::Copy, sizeof(Vertex));
			meshBuilder.End();

			meshes.Add(std::make_shared<Mesh>());
			meshBuilder.Begin();
			meshBuilder.AddYAxisCone(Vec3f(0.0f, orgSize, 0.0f), 0.05f, 0.1f, 0.05f, 0.1f, RGBA::GREEN);
			meshBuilder.CopyToMesh(renderer, *meshes.back(), &Vertex::BindVertexBuffer, &Vertex::Copy, sizeof(Vertex));
			meshBuilder.End();



			return true;

			}, RGBA::GREEN, RGBA::YELLOW, Gizmos::GizmoHandle::eHandle::TRANSLATE_Y)
	};

	if (!handles.Add(translationYHandle))
		assert(0);


	Gizmos::GizmoHandle translationZHandle {
		GizmoHandle(*this, [&](Gizmos::GizmoHandle& handle, Renderer& renderer) -> bool {
			Meshes& meshes = handle.GetMeshes();
			MeshBuilder meshBuilder;
			meshes.Add(std::make_shared<Mesh>());
			meshBuilder.Begin();
			meshBuilder.AddCylinder(Vec3f(0.0f, 0.0f, orgSize), Vec3f(0.01f, 0.0f, 0.0f), Vec3f(0.0f, 0.01f, 0.0f), 32, RGBA::BLUE);
			meshBuilder.CopyToMesh(renderer, *meshes.back(), &Vertex::BindVertexBuffer, &Vertex::Copy, sizeof(Vertex));
			meshBuilder.End();

			meshes.Add(std::make_shared<Mesh>());
			meshBuilder.Begin();
			meshBuilder.AddZAxisCone(Vec3f(0.0f, 0.0f, orgSize), 0.05f, 0.05f, 0.1f, 0.1f, RGBA::BLUE);
			meshBuilder.CopyToMesh(renderer, *meshes.back(), &Vertex::BindVertexBuffer, &Vertex::Copy, sizeof(Vertex));
			meshBuilder.End();
			
			return true;
			}, RGBA::BLUE, RGBA::YELLOW, Gizmos::GizmoHandle::eHandle::TRANSLATE_Z)
	};

	if (!handles.Add(translationZHandle))
		assert(0);

	Gizmos::GizmoHandle translationXYHandle{
		GizmoHandle(*this,[&](Gizmos::GizmoHandle& handle, Renderer& renderer)->bool {
			Meshes& meshes = handle.GetMeshes();

			MeshBuilder meshBuilder;
			meshes.Add(std::make_shared<Mesh>());
			meshBuilder.Begin();
			meshBuilder.AddCube(Vec3f(orgSize / 2.0f, orgSize / 2.0f, 0.0f), Vec3f(orgSize / 4.0f, orgSize / 4.0f, 0.01f), RGBA::WHITE);
			meshBuilder.CopyToMesh(renderer, *meshes.back(), &Vertex::BindVertexBuffer, &Vertex::Copy, sizeof(Vertex));
			meshBuilder.End();

			return true;
		}, RGBA::WHITE, RGBA::YELLOW, Gizmos::GizmoHandle::eHandle::TRANSLATE_XY)
	};

	if (!handles.Add(translationXYHandle))
		assert(0);

	Gizmos::GizmoHandle translationXZHandle{
		GizmoHandle(*this, [&](Gizmos::GizmoHandle& handle, Renderer& renderer)->bool {
			Meshes& meshes = handle.GetMeshes();

			MeshBuilder meshBuilder;
			meshes.Add(std::make_shared<Mesh>());
			meshBuilder.Begin();
			meshBuilder.AddCube(Vec3f(orgSize / 2.0f, 0.0f, orgSize / 2.0f), Vec3f(orgSize / 4.0f, 0.01f, orgSize / 4.0f), RGBA::WHITE);
			meshBuilder.CopyToMesh(renderer, *meshes.back(), &Vertex::BindVertexBuffer, &Vertex::Copy, sizeof(Vertex));
			meshBuilder.End();

			return true;
		}, RGBA::WHITE, RGBA::YELLOW, Gizmos::GizmoHandle::eHandle::TRANSLATE_XZ)
	};

	if (!handles.Add(translationXZHandle))
		assert(0);

	Gizmos::GizmoHandle translateYZHandle{
		GizmoHandle(*this, [&](Gizmos::GizmoHandle& handle, Renderer& renderer)->bool {
			Meshes& meshes = handle.GetMeshes();

			MeshBuilder meshBuilder;
			meshes.Add(std::make_shared<Mesh>());
			meshBuilder.Begin();
			meshBuilder.AddCube(Vec3f(0.0f, orgSize / 2.0f, orgSize / 2.0f), Vec3f(0.01f, orgSize / 4.0f, orgSize / 4.0f), RGBA::WHITE);
			meshBuilder.CopyToMesh(renderer, *meshes.back(), &Vertex::BindVertexBuffer, &Vertex::Copy, sizeof(Vertex));
			meshBuilder.End();

			return true;
		}, RGBA::WHITE, RGBA::YELLOW, Gizmos::GizmoHandle::eHandle::TRANSLATE_YZ)
	};


	if (!handles.Add(translateYZHandle))
		assert(0);

	Gizmos::GizmoHandle rotationXHandle{
		GizmoHandle(*this, [&](Gizmos::GizmoHandle& handle, Renderer& renderer) -> bool {
			Meshes& meshes = handle.GetMeshes();

			MeshBuilder meshBuilder;
			meshes.Add(std::make_shared<Mesh>());
			meshBuilder.Begin();
			meshBuilder.AddXAxisCircle(orgSize, orgSize - 0.05f, RGBA::RED);
			meshBuilder.CopyToMesh(renderer, *meshes.back(), &Vertex::BindVertexBuffer, &Vertex::Copy, sizeof(Vertex), Mesh::TriangleType::MeshType_Strip);
			meshBuilder.End();

			return true;
		}, RGBA::RED, RGBA::YELLOW, Gizmos::GizmoHandle::eHandle::ROTATE_X)
	};

	if (!handles.Add(rotationXHandle))
		assert(0);

	Gizmos::GizmoHandle rotationYHandle{
		GizmoHandle(*this, [&](Gizmos::GizmoHandle& handle, Renderer& renderer)->bool {
			Meshes& meshes = handle.GetMeshes();

			MeshBuilder meshBuilder;
			meshes.Add(std::make_shared<Mesh>());
			meshBuilder.Begin();
			meshBuilder.AddYAxisCircle(orgSize, orgSize - 0.05f, RGBA::GREEN);
			meshBuilder.CopyToMesh(renderer, *meshes.back(), &Vertex::BindVertexBuffer, &Vertex::Copy, sizeof(Vertex), Mesh::TriangleType::MeshType_Strip);
			meshBuilder.End();

			return true;
		}, RGBA::GREEN, RGBA::YELLOW, Gizmos::GizmoHandle::eHandle::ROTATE_Y)
	};

	if (!handles.Add(rotationYHandle))
		assert(0);


	Gizmos::GizmoHandle rotationZHandle{
		GizmoHandle(*this, [&](Gizmos::GizmoHandle& handle, Renderer& renderer)->bool {
			Meshes& meshes = handle.GetMeshes();

			MeshBuilder meshBuilder;
			meshes.Add(std::make_shared<Mesh>());
			meshBuilder.Begin();
			meshBuilder.AddZAxisCircle(orgSize, orgSize - 0.05f, RGBA::BLUE);
			meshBuilder.CopyToMesh(renderer, *meshes.back(), &Vertex::BindVertexBuffer, &Vertex::Copy, sizeof(Vertex), Mesh::TriangleType::MeshType_Strip);
			meshBuilder.End();

			return true;

		}, RGBA::BLUE, RGBA::YELLOW, Gizmos::GizmoHandle::eHandle::ROTATE_Z)
	};

	if (!handles.Add(rotationZHandle))
		assert(0);

	Gizmos::GizmoHandle scaleXHandle{
		GizmoHandle(*this, [&](Gizmos::GizmoHandle& handle, Renderer& renderer)->bool {
			Meshes& meshes = handle.GetMeshes();

			MeshBuilder meshBuilder;
			meshes.Add(std::make_shared<Mesh>());
			meshBuilder.Begin();
			meshBuilder.AddCylinder(Vec3f(orgSize, 0.0f, 0.0f), Vec3f(0.0f, 0.01f, 0.0f), Vec3f(0.0f, 0.0f, 0.01f), 32, RGBA::RED);
			meshBuilder.CopyToMesh(renderer, *meshes.back(), &Vertex::BindVertexBuffer, &Vertex::Copy, sizeof(Vertex));
			meshBuilder.End();

			meshes.Add(std::make_shared<Mesh>());
			meshBuilder.Begin();
			meshBuilder.AddCube(Vec3f(orgSize, 0.0f, 0.0f), Vec3f(0.05f, 0.05f, 0.05f), RGBA::RED);
			meshBuilder.CopyToMesh(renderer, *meshes.back(), &Vertex::BindVertexBuffer, &Vertex::Copy, sizeof(Vertex));
			meshBuilder.End();

			return true;
		}, RGBA::RED, RGBA::YELLOW, Gizmos::GizmoHandle::eHandle::SCALE_X)
	};
	
	if (!handles.Add(scaleXHandle)) {
		assert(0);
	}

	Gizmos::GizmoHandle scaleYHandle{
		GizmoHandle(*this, [&](Gizmos::GizmoHandle& handle, Renderer& renderer)->bool {
			Meshes& meshes = handle.GetMeshes();

			MeshBuilder meshBuilder;
			meshes.Add(std::make_shared<Mesh>());
			meshBuilder.Begin();
			meshBuilder.AddCylinder(Vec3f(0.0f, orgSize, 0.0f), Vec3f(0.0f, 0.0f, 0.01f), Vec3f(0.01f, 0.0f, 0.0f), 32, RGBA::GREEN);
			meshBuilder.CopyToMesh(renderer, *meshes.back(), &Vertex::BindVertexBuffer, &Vertex::Copy, sizeof(Vertex));
			meshBuilder.End();

			meshes.Add(std::make_shared<Mesh>());
			meshBuilder.Begin();
			meshBuilder.AddCube(Vec3f(0.0f, orgSize, 0.0f), Vec3f(0.05f, 0.05f, 0.05f), RGBA::GREEN);
			meshBuilder.CopyToMesh(renderer, *meshes.back(), &Vertex::BindVertexBuffer, &Vertex::Copy, sizeof(Vertex));
			meshBuilder.End();

			return true;
		}, RGBA::GREEN, RGBA::YELLOW, Gizmos::GizmoHandle::eHandle::SCALE_Y)
	};

	if (!handles.Add(scaleYHandle)) {
		assert(0);
	}

	Gizmos::GizmoHandle scaleZHandle{
		GizmoHandle(*this, [&](Gizmos::GizmoHandle& handle, Renderer& renderer)->bool {
			Meshes& meshes = handle.GetMeshes();

			MeshBuilder meshBuilder;
			meshes.Add(std::make_shared<Mesh>());
			meshBuilder.Begin();
			meshBuilder.AddCylinder(Vec3f(0.0f, 0.0f, orgSize), Vec3f(0.01f, 0.0f, 0.0f), Vec3f(0.0f, 0.01f, 0.0f), 32, RGBA::BLUE);
			meshBuilder.CopyToMesh(renderer, *meshes.back(), &Vertex::BindVertexBuffer, &Vertex::Copy, sizeof(Vertex));
			meshBuilder.End();

			meshes.Add(std::make_shared<Mesh>());
			meshBuilder.Begin();
			meshBuilder.AddCube(Vec3f(0.0f, 0.0f, orgSize), Vec3f(0.05f, 0.05f, 0.05f), RGBA::BLUE);
			meshBuilder.CopyToMesh(renderer, *meshes.back(), &Vertex::BindVertexBuffer, &Vertex::Copy, sizeof(Vertex));
			meshBuilder.End();

			return true;
		}, RGBA::BLUE, RGBA::YELLOW, Gizmos::GizmoHandle::eHandle::SCALE_Z)
	};

	if (!handles.Add(scaleZHandle)) {
		assert(0);
	}

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

double Gizmos::CalcDistanceFromCamera(const Camera& camera)const {
	Vec3f cameraPosition = camera.GetPosition();
	Vec3f position = GetPosition();
	Vec3d direction = cameraPosition - position;
	double distance = sqrt(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);
	return distance;
}

Vec3f Gizmos::CalcGizmoSize(const Camera& camera) const {
	float distance = (float)CalcDistanceFromCamera(camera);
	return orgSize * distance;
}

#ifdef _DEBUG
bool Gizmos::VerifyGizmos() const{
	if (!handles.DoesExist(GizmoHandle::eHandle::TRANSLATE_X))	return false;
	if (!handles.DoesExist(GizmoHandle::eHandle::TRANSLATE_Y))	return false;
	if (!handles.DoesExist(GizmoHandle::eHandle::TRANSLATE_Z))	return false;
	if (!handles.DoesExist(GizmoHandle::eHandle::TRANSLATE_XY))	return false; 
	if (!handles.DoesExist(GizmoHandle::eHandle::TRANSLATE_XZ)) return false;
	if (!handles.DoesExist(GizmoHandle::eHandle::TRANSLATE_YZ)) return false;
	if (!handles.DoesExist(GizmoHandle::eHandle::ROTATE_X))		return false;
	if (!handles.DoesExist(GizmoHandle::eHandle::ROTATE_Y))		return false;
	if (!handles.DoesExist(GizmoHandle::eHandle::ROTATE_Z))		return false;
	if (!handles.DoesExist(GizmoHandle::eHandle::SCALE_X))		return false;
	if (!handles.DoesExist(GizmoHandle::eHandle::SCALE_Y))		return false;
	if (!handles.DoesExist(GizmoHandle::eHandle::SCALE_Z))		return false;

	return true;
}
#endif



bool Gizmos::Intersect(const Ray& ray, double& distance) {
	if (targets.empty())
		return false;

	Gizmos::eTransformMode transformMode = GetMode();	
	//기즈모 스케일 처리가 들어가야함
	GizmoHandle::eHandle handleType = GizmoHandle::eHandle::NONE;
	switch (transformMode) {
	case Gizmos::eTransformMode::TRANSLATE: handleType = handles.Intersect(ray, distance, transformMode);	break;
	case Gizmos::eTransformMode::ROTATE:	handleType = handles.Intersect(ray, distance, transformMode);	break;
	case Gizmos::eTransformMode::SCALE:		handleType = handles.Intersect(ray, distance, transformMode);   break;
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

	transform->SetPosition(targets[0]->GetPosition());
	transform->SetScale(CalcGizmoSize(*pCamera));
	pCamera->GetViewMatrix(shaderParam.viewMatrix);

	Matrix<float, 4, 4> worldMatrix = transform->GetWorldMatrix();
	Matrix<float, 4, 4> worldViewMatrix = Matrix<float, 4, 4>::Identity();
	MakeWorldViewMatrix(worldMatrix, shaderParam.viewMatrix, worldViewMatrix);
	
	shaderParam.worldViewMatrix = worldViewMatrix;
	shaderParam.projectionMatrix = pCamera->GetFrustum();

	shaderParam.focusInfo.focusObjNo = (int)context.editingHandle;
	shaderParam.focusInfo.highlightColor = Vec4f(1.0f, 0.8f, 0.0f, 1.0f);
	

	handles.Render(renderer, shaderParam, *pCamera, GetMode());
}