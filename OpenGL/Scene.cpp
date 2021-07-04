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
	 pShader(nullptr),
	 projectionMatrix()
{
}

Scene::~Scene() {
	if (pShader) {
		delete pShader;
		pShader = nullptr;
	}
	if (pCamera) {
		delete pCamera;
		pCamera = nullptr;
	}
}

bool Scene::BuildObject(Renderer& renderer) {	
	phongLight.SetPosition(0.0f, 0.0f, -100.0f);
	phongLight.SetDirection(Vec3f::FORWARD);

	pShader = new PhongShader();
	if (!pShader)
		return false;

	bool result = pShader->Initialize(renderer);
	if (!result) {
		LogError(L"Could not initialize the shader object.");
		return false;
	}

	pCamera = new Camera();
	if (!pCamera) {
		assert(0);
		return false;
	}

	pCamera->SetPosition(0.0f, 0.0f, -10.0f);
	pCamera->SetWidth(renderer.GetRenderTargetWidth());
	pCamera->SetHeight(renderer.GetRenderTargetHeight());

	return true;
}

void Scene::Update(const float& elapsedTime) {
	if (pCamera)
		pCamera->Update(elapsedTime);

	if (pShader) {
		pShader->Update(elapsedTime);
	}
}

bool Scene::Render(Renderer& renderer) {
	renderer.BeginRender();

	Vec3f lightPosition = phongLight.GetPosition();
	Vec3f lightDirection = phongLight.GetDirection();
	
	worldMatrix = Matrix<float, 4, 4>::Identity();

	pCamera->BuildPerspectiveFovLHMatrix(projectionMatrix, SCREEN_NEAR, SCREEN_DEPTH);

	Matrix<float, 4, 4> viewMatrix = GetViewMatrix();
	Vec3f cameraPosition = pCamera->GetPosition();
	pShader->Render(renderer, viewMatrix, projectionMatrix, lightPosition, cameraPosition);

	renderer.EndRender();
	return true;
}

void Scene::Shutdown(Renderer& renderer) {
	if (pShader)
		pShader->Shutdown(renderer);
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
	return pShader->GetObjectCount();
}
void Scene::Picking(int x, int y, int screenWidth, int screenHeight) {
	RayCast rayCast(*this);

	Object* pHitObject = rayCast.HitTest(x, y, screenWidth, screenHeight);
	if (pHitObject) {

	}
}


bool Scene::IntersectObjects(const Ray& ray) const {
	return pShader->IntersectObjects(ray);
}


