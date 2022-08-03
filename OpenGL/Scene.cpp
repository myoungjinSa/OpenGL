#include "Common.h"
#include "Shader.h"
#include "Scene.h"
#include "Renderer.h"
#include "RayCast.h"
#include "Logger.h"
#include "Material.h"
#include "Terrain.h"
#include "Light.h"
#include "Texture.h"

Scene::Scene() 
	: gizmos()
{

}

Scene::~Scene() {

	gameObjects.Clear();
}

bool Scene::BuildObject(Renderer& renderer) {	
	skybox.Initialize(renderer);
	phongLight = std::make_unique<Light>();
	phongLight->SetPosition(0.0f, 0.0f, -5.0f);
	phongLight->SetDirection(Vec3f::FORWARD);

	terrain = std::make_unique<Terrain>(10, 10);
	terrain->Initialize(renderer);

	GameObject* pObject = new Sphere(1.0f, 32, 32);//new Cylinder(Vec3f::UP, Vec3f::RIGHT, Vec3f::FORWARD, 32);
	gameObjects.Add(*pObject);

	for (const auto& obj : gameObjects) {
		if (!obj->Initialize(renderer)) {
			LogError(L"Could not initialize the model object\n");
			return false;
		}
	}

	camera.Initialize(renderer);
	camera.SetPosition(0.0f, 0.0f, -5.0f);
	camera.SetViewport(Recti(0.0f, 0.0f, renderer.GetScreenSize().width, renderer.GetScreenSize().height));
	camera.SetNear(1.0f);
	camera.SetFar(1000.0f);
	gizmos.Initialize(renderer);
	
	sceneSize = Size2u(renderer.GetScreenSize().width, renderer.GetScreenSize().height);
	
	renderTarget = std::make_shared<RenderTarget>();
	if (!renderer.CreateRenderTarget(*renderTarget, sceneSize, false))
		assert(0);
	
	offscreenShader = std::make_shared<PostProcessingShader>(this);
	if (!offscreenShader->Initialize(renderer))
		return false;

	return true;
}

void Scene::Update(double elapsedTime) {
	camera.Update(elapsedTime);

	terrain->Update(elapsedTime);
	for (const auto& obj : gameObjects) {
		obj->Update(elapsedTime);
		//LogDebug(L"Position = %.8lf, %.8lf, %.8lf\n", obj->GetPosition().x, obj->GetPosition().y, obj->GetPosition().z);
		//LogDebug(L"Look = %.8lf, %.8lf, %.8lf\n", obj->GetLook().x, obj->GetLook().y, obj->GetLook().z);
		//LogDebug(L"Up = %.8lf, %.8lf, %.8lf\n", obj->GetUp().x, obj->GetUp().y, obj->GetUp().z);
		//LogDebug(L"Right = %.8lf, %.8lf, %.8lf\n", obj->GetRight().x, obj->GetRight().y, obj->GetRight().z);
		//LogDebug(L"Scale = %.8lf, %.8lf, %.8lf\n", obj->GetScale().x, obj->GetScale().y, obj->GetScale().z);
		
		//obj->Rotate(MathUtils::DegreesToRadians(0.0f), MathUtils::DegreesToRadians(1.0f), MathUtils::DegreesToRadians(0.0f));
		//obj->Move(obj->GetLook(), 1.0f, elapsedTime);
	}
}

bool Scene::Render(Renderer& renderer) {
	renderer.BindRenderTarget(*renderTarget);
	renderer.BeginRender();
	Size2u screenSize = renderer.GetScreenSize();

	float width = screenSize.width;
	float height = screenSize.height;

	camera.SetFrustum(width, height, camera.GetNear(), camera.GetFar(), MathUtils::DegreesToRadians(60));
	Matrix<float, 4, 4> projectionMatrix = camera.GetFrustum();
	
	renderer.EnableDepthTest(true);
	renderer.SetCullFace(eFace::Back);
	renderer.SetDepthFunc(eCompare::LEQUAL);
	ShaderParameter shaderParmaeter;
	renderer.SetWindingOrder(Renderer::WindingOrder::CW);
	skybox.FillShaderParameter(shaderParmaeter, GetViewMatrix(), projectionMatrix, *phongLight, camera, 0);
	skybox.Render(renderer, shaderParmaeter);
	renderer.SetDepthFunc(eCompare::LESS);

	renderer.SetDrawMode(Renderer::DrawMode::TRIANGLE_STRIP);
	terrain->FillShaderParameter(shaderParmaeter, GetViewMatrix(), projectionMatrix, *phongLight, camera, 0);
	terrain->Render(renderer, shaderParmaeter);
	renderer.SetDrawMode(Renderer::DrawMode::TRIANGLES);

	for (size_t iObj = 0; iObj < gameObjects.size(); iObj++) {
		ShaderParameter shaderParmaeter;
		gameObjects[iObj]->FillShaderParameter(shaderParmaeter, GetViewMatrix(), projectionMatrix, *phongLight, camera, iObj);
		gameObjects[iObj]->Render(renderer, shaderParmaeter);
	}

	renderer.EnableDepthTest(false);
	gizmos.Render(renderer, &camera, *this);
	renderer.EnableDepthTest(true);

	renderer.UnbindRenderTarget(*renderTarget);
	renderTarget->Render(renderer, offscreenShader);

	renderer.EndRender();
	return true;
}

void Scene::Shutdown(Renderer& renderer) {
	renderer.DestroyRenderTarget(*renderTarget);

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
	return camera.GetFrustum();
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

void Scene::SetTransformMode(Gizmos::eTransformMode transformMode) {
	gizmos.SetTransformMode(transformMode);
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

