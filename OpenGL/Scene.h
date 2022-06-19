#pragma once
#include <vector>
#include "Types.h"
#include "Object.h"
#include "Gizmos.h"
#include "Camera.h"
#include "RayCast.h"

class Light;
class Terrain;
class Shader;
class ShaderParameter;
class Renderer;
class Material;
class Scene;
class GameObjectPicker {
public:
	struct sPickedInfo {
		sPickedInfo(GameObject* _pObject, float _Distance);

		GameObject* pObject;
		float   distance;
	};


	typedef std::list<sPickedInfo> PickedList;

	GameObjectPicker();
	GameObjectPicker(const Ray& ray , float _near, float _far);

	bool HitTest(Gizmos& gizmos, double& distance);
	bool HitTest(GameObject& obj, double& distance);
	bool HitTest(const Scene& scene);
	void Clear();

	void SetRay(const Ray& _ray) { if(ray.IsValid()) ray = _ray; }
	void SetNear(float _near) { Near = _near; }
	void SetFar(float _far) { Far = _far; }

	GameObjects GetSelectedObjects()const;
private:
	Ray			ray;
	PickedList	pickedList;
	float		Near;
	float       Far;

	PickedList GetPickedList() const { return pickedList; }

	bool InsertPicked(GameObject& object, float distance);
	bool IsAlreadyExist(const GameObject& object) const;
};


class Scene final : public Object
{
	friend class SceneEdit;
public:
	Scene();
	~Scene();

	bool BuildObject(Renderer& gl);
	void Update(double elapsedTime);
	bool Render(Renderer& renderer);
	void Shutdown(Renderer& gl);

	Matrix<float, 4, 4> GetViewMatrix() const;
	Matrix<float, 4, 4> GetProjectionMatrix() const;
	
	size_t	GetObjectCount() const;
	GameObject* GetGameObject(size_t iIndex) const;

	Ray		GetRay(const Point2i& pt, const Size2u& size)const;
	Ray     GetRay(int x, int y, size_t screenWidth, size_t screneHeight) const;
	bool	IntersectObjects(const Ray& ray) const;
	
	GameObject* GetGameObject(uint32_t idx) const;

	Vec3f GetCameraPosition()const;
	const Camera& GetCamera() const { return camera; }

	Size2u GetSceneSize() const { return sceneSize; }

	void SetTransformMode(Gizmos::eTransformMode transformMode);
private:
	Size2u sceneSize;
	Camera camera;
	Gizmos gizmos;

	GameObjects gameObjects;
	std::unique_ptr<Light> phongLight;
	Cubemap skybox;
	std::unique_ptr<Terrain> terrain;
};
