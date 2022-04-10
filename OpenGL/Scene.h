#pragma once
#include <vector>
#include "Light.h"
#include "Matrix.h"
#include "Object.h"
#include "Gizmos.h"
#include "Camera.h"
#include "RayCast.h"

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
	PickedList GetPickedList() const { return pickedList; }

	bool HitTest(const Scene& scene);
	void Clear();

	void SetRay(const Ray& _ray) { if(ray.IsValid()) ray = _ray; }
	void SetNear(float _near) { Near = _near; }
	void SetFar(float _far) { Far = _far; }
private:
	Ray			ray;
	PickedList	pickedList;
	float		Near;
	float       Far;

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

	Ray     GetRay(int x, int y, int screenWidth, int screneHeight) const;
	bool	IntersectObjects(const Ray& ray) const;
	
	GameObject* GetGameObject(uint32_t idx) const;

	Vec3f GetCameraPosition()const;
	const Camera& GetCamera() const { return camera; }

private:
	Camera camera;
	Gizmos Gizmo;

	//std::shared_ptr<Shader> DefaultShader;
	std::vector<GameObject*> objects;
	Light phongLight;
	Cubemap skybox;

	Matrix<float, 4, 4> projectionMatrix;
};
