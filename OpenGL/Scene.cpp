#include "Common.h"
#include "Shader.h"
#include "Camera.h"
#include "Scene.h"
#include "Object.h"
#include "Renderer.h"
#include "RayCast.h"
#include "Logger.h"

Scene::Scene() 
	:pCamera(nullptr),
	 worldMatrix(),
	 projectionMatrix()
{
	shaders.clear();
}

Scene::~Scene() {
	while (!shaders.empty()) {
		shaders.pop_back();
	}
	if (pCamera) {
		delete pCamera;
		pCamera = nullptr;
	}
}

bool Scene::BuildObject(Renderer& renderer) {	
	phongLight.SetPosition(0.0f, 0.0f, -10.0f);
	phongLight.SetDirection(Vec3f::FORWARD);

	shaders.push_back(std::make_shared<PhongShader>());
	shaders.push_back(std::make_shared<ColorShader>());

	for (auto shader : shaders) {
		if (!shader->Initialize(renderer)) {
			LogError(L"Could not initialize the shader object.");
			return false;
		}
	}

	pCamera = new Camera();
	if (!pCamera) {
		assert(0);
		return false;
	}

	pCamera->Initialize(renderer);
	pCamera->SetPosition(0.0f, 0.0f, -10.0f);
	pCamera->SetViewport(Rect2f(0.0f, 0.0f, renderer.GetRenderTargetWidth(), renderer.GetRenderTargetHeight()));
	
	return true;
}

void Scene::Update(const float& elapsedTime) {
	if (pCamera)
		pCamera->Update(elapsedTime);

	for (auto shader : shaders) {
		shader->Update(elapsedTime);
	}
}

bool Scene::Render(Renderer& renderer) {
	renderer.BeginRender();

	Vec3f lightPosition = phongLight.GetPosition();
	Vec3f lightDirection = phongLight.GetDirection();
	
	worldMatrix = Matrix<float, 4, 4>::Identity();

	pCamera->BuildPerspectiveFovLHMatrix(projectionMatrix, pCamera->GetViewport(), SCREEN_NEAR, SCREEN_DEPTH);

	Matrix<float, 4, 4> viewMatrix = GetViewMatrix();
	Vec3f cameraPosition = pCamera->GetPosition();
	shaders[0]->Render(renderer, viewMatrix, projectionMatrix, lightPosition, cameraPosition);
	shaders[1]->Render(renderer, viewMatrix, projectionMatrix);

	renderer.EndRender();
	return true;
}

void Scene::Shutdown(Renderer& renderer) {
	for (auto shader : shaders)
		shader->Shutdown(renderer);

	if (pCamera)
		pCamera->Shutdown(renderer);
}


Matrix<float, 4, 4> Scene::GetWorldMatrix() const {
	return worldMatrix;
}
Matrix<float, 4, 4> Scene::GetViewMatrix() const {
	Matrix<float, 4, 4> viewMatrix;
	if (pCamera)
		pCamera->GetViewMatrix(viewMatrix);

	return viewMatrix;
}
Matrix<float, 4, 4> Scene::GetProjectionMatrix() const {
	return projectionMatrix;
}



size_t Scene::GetObjectCount() const {
	return shaders[0]->GetObjectCount();
}
void Scene::Picking(int x, int y, int screenWidth, int screenHeight) {
	RayCast rayCast(*this);

	Object* pHitObject = rayCast.HitTest(x, y, screenWidth, screenHeight);
	if (pHitObject) {

	}
}


bool Scene::IntersectObjects(const Ray& ray) const {
	return shaders[0]->IntersectObjects(ray);
}


