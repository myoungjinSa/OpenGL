#include "Common.h"
#include "Shader.h"
#include "Scene.h"
#include "Renderer.h"
#include "RayCast.h"
#include "Logger.h"
#include "Material.h"
#include "Terrain.h"
#include "Light.h"

Scene::Scene() 
	:projectionMatrix(),
	 gizmos()
{

}

Scene::~Scene() {

	gameObjects.Clear();
}

bool Scene::BuildObject(Renderer& renderer) {	
	skybox.Initialize(renderer);
	phongLight = std::make_unique<Light>();
	phongLight->SetPosition(0.0f, 0.0f, -10.0f);
	phongLight->SetDirection(Vec3f::FORWARD);

	terrain = std::make_unique<Terrain>(100, 100);
	terrain->Initialize(renderer);

	GameObject* pObject = new Cube(Vec3f(1.0f, 1.0f, 1.0f));//new Cylinder(Vec3f::UP, Vec3f::RIGHT, Vec3f::FORWARD, 32);
	gameObjects.Add(*pObject);

	for (const auto& obj : gameObjects) {
		if (!obj->Initialize(renderer)) {
			LogError(L"Could not initialize the model object\n");
			return false;
		}
	}

	camera.Initialize(renderer);
	camera.SetPosition(0.0f, 0.0f, -5.0f);
	camera.SetViewport(Rect2f(0.0f, 0.0f, renderer.GetRenderTargetWidth(), renderer.GetRenderTargetHeight()));
	camera.SetNear(1.0f);
	camera.SetFar(1000.0f);
	gizmos.Initialize(renderer);
	
	sceneSize = Size2u(renderer.GetRenderTargetWidth(), renderer.GetRenderTargetHeight());
	return true;
}

void Scene::Update(double elapsedTime) {
	camera.Update(elapsedTime);

	terrain->Update(elapsedTime);
	for (const auto& obj : gameObjects) {
		obj->Update(elapsedTime);
		//obj->Rotate(MathUtils::DegreesToRadians(0.0f), MathUtils::DegreesToRadians(1.0f), MathUtils::DegreesToRadians(0.0f));
		//obj->Move(obj->GetLook(), 1.0f, elapsedTime);
	}
}

bool Scene::Render(Renderer& renderer) {
	renderer.BeginRender();
	camera.BuildPerspectiveFovLHMatrix(projectionMatrix);
	
	renderer.SetCullingMode(Renderer::CullingMode::Back);
	renderer.SetDepthFunc(GL_LEQUAL);
	ShaderParameter shaderParmaeter;
	renderer.SetWindingOrder(Renderer::WindingOrder::CW);
	skybox.FillShaderParameter(shaderParmaeter, GetViewMatrix(), projectionMatrix, *phongLight, camera, 0);
	skybox.Render(renderer, shaderParmaeter);
	renderer.SetDepthFunc(GL_LESS);

	renderer.SetDrawMode(Renderer::DrawMode::TRIANGLE_STRIP);
	renderer.SetRenderMode(Renderer::RenderMode::WIREFRAME);
	terrain->FillShaderParameter(shaderParmaeter, GetViewMatrix(), projectionMatrix, *phongLight, camera, 0);
	terrain->Render(renderer, shaderParmaeter);
	renderer.SetRenderMode(Renderer::RenderMode::SOLID);
	renderer.SetDrawMode(Renderer::DrawMode::TRIANGLES);

	Matrix<float, 4, 4> worldMatrix;
	for (size_t iObj = 0; iObj < gameObjects.size(); iObj++) {
		ShaderParameter shaderParmaeter;
		gameObjects[iObj]->FillShaderParameter(shaderParmaeter, GetViewMatrix(), projectionMatrix, *phongLight, camera, iObj);
		gameObjects[iObj]->Render(renderer, shaderParmaeter);
	}

	renderer.SetDepthTest(false);
	gizmos.Render(renderer, &camera, *this);
	renderer.SetDepthTest(true);

	renderer.EndRender();
	return true;
}

void Scene::Shutdown(Renderer& renderer) {
	for (const auto& obj : gameObjects)
		obj->Shutdown(renderer);

	terrain->Shutdown(renderer);
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
	if (gameObjects.size() <= idx)
		return nullptr;

	return gameObjects[idx];
}

Vec3f Scene::GetCameraPosition() const {
	return camera.GetPosition();
}
size_t Scene::GetObjectCount() const {
	return gameObjects.size();
}
GameObject* Scene::GetGameObject(size_t iIndex) const {
	return gameObjects.at(iIndex);
}

Ray Scene::GetRay(const Point2i& pt, const Size2u& size) const {
	return GetRay(pt.x, pt.y, size.width, size.height);
}

Ray Scene::GetRay(int x, int y, size_t screenWidth, size_t screenHeight) const{
	RayCast raycast;
	Ray ray = raycast.GetRay(x, y, screenWidth, screenHeight, GetViewMatrix(), GetProjectionMatrix());
	return ray;
}

bool Scene::IntersectObjects(const Ray& ray) const {
	double distance = 0.0;
	for (size_t iObj = 0; iObj < gameObjects.size(); iObj++) {
		if (gameObjects[iObj]->Intersect(ray, distance))
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

bool GameObjectPicker::HitTest(Gizmos& gizmos, double& distance) {
	if (gizmos.Intersect(ray, distance)) {
		return true;
	}
	return false;
}
bool GameObjectPicker::HitTest(GameObject& obj, double& distance) {
	if (obj.Intersect(ray, distance)) {
		InsertPicked(obj, distance);
		return true;
	}
	return false;
}
bool GameObjectPicker::HitTest(const Scene& scene) {
	bool bPicked = false;
	for (size_t iObj = 0; iObj < scene.GetObjectCount(); iObj++) {
		GameObject* pGameObject = scene.GetGameObject(iObj);
		if (!pGameObject)
			continue;

		double distance = 0.0;
		bPicked = HitTest(*pGameObject, distance);
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

GameObjects GameObjectPicker::GetSelectedObjects()const {
	GameObjects gameObjects;
	for (PickedList::const_iterator iPicked = pickedList.begin(); iPicked != pickedList.end(); iPicked++) {
		const sPickedInfo& pickedInfo = *iPicked;
		gameObjects.push_back(pickedInfo.pObject);
	}
	return gameObjects;
}

