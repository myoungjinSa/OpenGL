#include "Common.h"
#include "Shader.h"
#include "Camera.h"
#include "Scene.h"
#include "Object.h"
#include "Renderer.h"

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

bool Scene::BuildObject(Renderer& renderer, HWND hWnd) {	
	Vec4f diffuseColor(1.0f, 0.9f, 0.9f, 1.0f);
	Vec4f ambientColor(0.15f, 0.15f, 0.15f, 1.0f);
	Vec4f specularColor(1.0f, 1.0f, 0.5f, 1.0f);

	phongLight.SetDiffuseColor(diffuseColor);
	phongLight.SetAmbientLight(ambientColor);
	phongLight.SetSpecularLight(specularColor);
	phongLight.SetPosition(0.0f, 0.0f, -20.0f);
	phongLight.SetDirection(Vec3f::FORWARD);

	pShader = new PhongShader();
	if (!pShader)
		return false;

	bool result = pShader->Initialize(renderer, hWnd);
	if (!result) {
		MessageBox(hWnd, L"Could not initialize the shader object.", L"Error", MB_OK);
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
	Vec3f diffuseAlbedo = Vec3f(phongLight.GetDiffuseLight().x, phongLight.GetDiffuseLight().y, phongLight.GetDiffuseLight().z);
	Vec4f ambientAlbedo = phongLight.GetAmbientLight();
	Vec3f specularAlbedo = Vec3f(phongLight.GetSpecularLight().x, phongLight.GetSpecularLight().y, phongLight.GetSpecularLight().z);
	Vec3f lightPosition = phongLight.GetPosition();
	Vec3f lightDirection = phongLight.GetDirection();
	
	worldMatrix = Matrix<float, 4, 4>::Identity();

	pCamera->BuildPerspectiveFovLHMatrix(projectionMatrix, SCREEN_NEAR, SCREEN_DEPTH);

	Matrix<float, 4, 4> viewMatrix = GetViewMatrix();
	pShader->SetShader(renderer);
	pShader->SetShaderParameters(renderer, worldMatrix, viewMatrix, projectionMatrix, lightDirection, diffuseAlbedo, ambientAlbedo, specularAlbedo, 0);
	pShader->Render(renderer);

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
