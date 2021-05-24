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
	pCamera->SetPosition(0.0f, 0.0f, -10.0f);
	return true;
}
void Scene::Prepare(Renderer& renderer) {
	pCamera->Update();
}

void Scene::Update(const float& elapsedTime) {
	
}

bool Scene::Render(Renderer& renderer) {
	renderer.BeginRender();

	const int columnCount = 4;
	std::array<Vec4f, columnCount> worldMatrix;
	std::array<Vec4f, columnCount> viewMatrix;
	std::array<Vec4f, columnCount> projectionMatrix;
	for (size_t iRow = 0; iRow < columnCount; iRow++) {
		worldMatrix.fill(GetWorldMatrix().Row(iRow));
		viewMatrix.fill(GetViewMatrix().Row(iRow));
		projectionMatrix.fill(GetProjectionMatrix().Row(iRow));
	}
	
	float diffuseAlbedo[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	float ambientAlbedo[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	float specularAlbedo[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	float lightPosition[3] = { 0.0f, 0.0f, 0.0f };
	float lightDirection[3] = { 0.0f, 0.0f, 0.0f };

	Vec4f diffuseAlbedo = phongLight.GetDiffuseLight();
	Vec4f ambientAlbedo = phongLight.GetAmbientLight();
	Vec4f specularAlbedo = phongLight.GetSpecularLight();
	Vec3f lightPosition = phongLight.GetPosition();
	Vec3f lightDirection = phongLight.GetDirection();

	pShader->SetShader(renderer);

	//pShader->SetShaderParameters(renderer, )
	//pShader->SetShaderParameters(gl, (float*)worldMatrix.value, (float*)viewMatrix.value, (float*)projectionMatrix.value, 0, lightDirection, diffuseAlbedo, ambientAlbedo, specularAlbedo);
	pShader->Render(renderer);

	renderer.EndRender();
	return true;
}

void Scene::Shutdown(Renderer& renderer) {
	if (pShader)
		pShader->Shutdown(renderer);
}


const Matrix<float, 4, 4>& Scene::GetWorldMatrix() const {
	return worldMatrix;
}

const Matrix<float, 4, 4>& Scene::GetViewMatrix() const {
	Matrix<float, 4, 4> viewMatrix;
	if (pCamera)
		pCamera->GetViewMatrix(viewMatrix);

	return viewMatrix;
}
const Matrix<float, 4, 4>& Scene::GetProjectionMatrix() const {
	return projectionMatrix;
}
