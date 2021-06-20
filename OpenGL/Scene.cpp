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
}

bool Scene::Render(Renderer& renderer) {
	renderer.BeginRender();

	Vec3f lightPosition = phongLight.GetPosition();
	Vec3f lightDirection = phongLight.GetDirection();
	
	worldMatrix = Matrix<float, 4, 4>::Identity();

	pCamera->BuildPerspectiveFovLHMatrix(projectionMatrix, SCREEN_NEAR, SCREEN_DEPTH);

	Matrix<float, 4, 4> viewMatrix = GetViewMatrix();

	Matrix<float, 4, 4> testMatrix;
	testMatrix.value[0] = 1;
	testMatrix.value[1] = 0;
	testMatrix.value[2] = 3;
	testMatrix.value[3] = 7;
	testMatrix.value[4] = 4;
	testMatrix.value[5] = 2;
	testMatrix.value[6] = 0;
	testMatrix.value[7] = 1;
	testMatrix.value[8] = 7;
	testMatrix.value[9] = 7;
	testMatrix.value[10] = 3;
	testMatrix.value[11] = 0;
	testMatrix.value[12] = 5;
	testMatrix.value[13] = 0;
	testMatrix.value[14] = 6;
	testMatrix.value[15] = 8;

	Matrix<float, 4, 4> invView = Inverse(testMatrix);
	
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

void Scene::Picking(int x, int y) {
	RayCast rayCast(*this);

	Object* pHitObject = rayCast.HitTest(x, y);
	if (pHitObject) {

	}
}
