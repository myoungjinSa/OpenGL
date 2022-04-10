#include "Common.h"
#include "Shader.h"
#include "Scene.h"
#include "Renderer.h"
#include "RayCast.h"
#include "Logger.h"
#include "Material.h"



Scene::Scene() 
	:projectionMatrix(),
	 Gizmo()
{

}

Scene::~Scene() {

	for (auto& obj : objects) {
		delete obj;
	}
	objects.clear();
}

bool Scene::BuildObject(Renderer& renderer) {	
	skybox.Initialize(renderer);
	phongLight.SetPosition(0.0f, 0.0f, -10.0f);
	phongLight.SetDirection(Vec3f::FORWARD);

	GameObject* pObject = new Cube(Vec3f(1.0f, 1.0f, 1.0f));//new Cylinder(Vec3f::UP, Vec3f::RIGHT, Vec3f::FORWARD, 32);
	objects.emplace_back(pObject);
	
	for (const auto& obj : objects) {
		if (!obj->Initialize(renderer)) {
			LogError(L"Could not initialize the model object\n");
			return false;
		}
	}

	camera.Initialize(renderer);
	camera.SetPosition(0.0f, 0.0f, -5.0f);
	camera.SetViewport(Rect2f(0.0f, 0.0f, renderer.GetRenderTargetWidth(), renderer.GetRenderTargetHeight()));
	camera.SetNear(1.0f);
	camera.SetFar(100000.0f);
	Gizmo.Initialize(renderer);

	return true;
}

void Scene::Update(double elapsedTime) {
	camera.Update(elapsedTime);

	Gizmo.Update(camera, elapsedTime);
	for (const auto& obj : objects) {
		obj->Update(elapsedTime);
		obj->Rotate(MathUtils::DegreesToRadians(0.0f), MathUtils::DegreesToRadians(1.0f), MathUtils::DegreesToRadians(0.0f));
		//obj->Move(obj->GetLook(), 1.0f, elapsedTime);
	}
}

bool Scene::Render(Renderer& renderer) {
	renderer.BeginRender();
	camera.BuildPerspectiveFovLHMatrix(projectionMatrix);

	renderer.SetDepthFunc(GL_LEQUAL);
	ShaderParameter shaderParmaeter;
	skybox.FillShaderParameter(shaderParmaeter, GetViewMatrix(), GetProjectionMatrix(), phongLight, camera);
	skybox.Render(renderer, shaderParmaeter);
	renderer.SetDepthFunc(GL_LESS);

	Matrix<float, 4, 4> worldMatrix;
	for (size_t iObj = 0; iObj < objects.size(); iObj++) {
		ShaderParameter shaderParmaeter;
		objects[iObj]->FillShaderParameter(shaderParmaeter, GetViewMatrix(), GetProjectionMatrix(), phongLight, camera);
		objects[iObj]->Render(renderer, shaderParmaeter);
	}

	renderer.SetDepthTest(false);
	Gizmo.Render(renderer, &camera, *this);
	renderer.SetDepthTest(true);

	renderer.EndRender();
	return true;
}

void Scene::Shutdown(Renderer& renderer) {
	for (const auto& obj : objects)
		obj->Shutdown(renderer);

	skybox.Shutdown(renderer);

	camera.Shutdown(renderer);
}

Matrix<float, 4, 4> Scene::GetViewMatrix() const {
	Matrix<float, 4, 4> viewMatrix;
	camera.GetViewMatrix(viewMatrix);

	return viewMatrix;
}
Matrix<float, 4, 4> Scene::GetProjectionMatrix() const {
	return projectionMatrix;
}

GameObject* Scene::GetGameObject(uint32_t idx) const{
	if (objects.size() <= idx)
		return nullptr;

	return objects[idx];
}

Vec3f Scene::GetCameraPosition() const {
	return camera.GetPosition();
}
size_t Scene::GetObjectCount() const {
	return objects.size();
}
GameObject* Scene::GetGameObject(size_t iIndex) const {
	return objects.at(iIndex);
}

Ray Scene::GetRay(int x, int y, int screenWidth, int screenHeight) const{
	RayCast raycast;
	Ray ray = raycast.GetRay(x, y, screenWidth, screenHeight, GetViewMatrix(), GetProjectionMatrix());
	return ray;
}

bool Scene::IntersectObjects(const Ray& ray) const {
	double distance = 0.0;
	for (size_t iObj = 0; iObj < objects.size(); iObj++) {
		if (objects[iObj]->Intersect(ray, distance))
			return true;
	}
	return false;
}

GameObjectPicker::sPickedInfo::sPickedInfo(GameObject* _pObject, float _distance)
	: pObject(_pObject)
	, distance(_distance)
{
}

GameObjectPicker::GameObjectPicker() {

}
GameObjectPicker::GameObjectPicker(const Ray& _ray, float _near, float _far)
	: ray(_ray)
	, Near(_near)
	, Far(_far)
{

}
void GameObjectPicker::Clear() {
	pickedList.clear();
}
bool GameObjectPicker::HitTest(const Scene& scene) {
	bool bPicked = false;
	for (size_t iObj = 0; iObj < scene.GetObjectCount(); iObj++) {
		GameObject* pGameObject = scene.GetGameObject(iObj);
		if (!pGameObject)
			continue;

		double distance = 0.0;
		if (pGameObject->Intersect(ray, distance)) {
			InsertPicked(*pGameObject, distance);
			bPicked = true;
		}
	}

	return bPicked;
}

bool GameObjectPicker::InsertPicked(GameObject& object, float distance) {
	if (IsAlreadyExist(object))
		return false;
		
	if (pickedList.empty()) {
		pickedList.push_back(sPickedInfo(&object, distance));
	}else {

	}
	return true;
}

bool GameObjectPicker::IsAlreadyExist(const GameObject& object) const{
	for (PickedList::const_iterator iPicked = pickedList.begin(); iPicked != pickedList.end(); iPicked++) {
		const sPickedInfo& pickedInfo = *iPicked;
		if (pickedInfo.pObject == &object) {
			return true;
		}
	}
	return false;
}



