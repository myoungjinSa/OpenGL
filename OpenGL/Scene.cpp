#include "Shader.h"
#include "Camera.h"
#include "Scene.h"
#include "Object.h"

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

bool Scene::BuildObject(OpenGL& gl, HWND hWnd) {	
	Vec4f lightColor(1.0f, 0.7f, 1.0f, 1.0f);
	diffuseLight.SetDiffuseColor(lightColor);
	diffuseLight.SetDirection(Vec3f::FORWARD);

	pShader = new TextureShader();
	if (!pShader)
		return false;

	bool result = pShader->Initialize(gl, hWnd);
	if (!result) {
		MessageBox(hWnd, L"Could not initialize the shader object.", L"Error", MB_OK);
		return false;
	}

	pCamera = new Camera();
	pCamera->SetPosition(5.0f, 3.0f, -10.0f);
	return true;
}
void Scene::Prepare(OpenGL& gl) {
	pCamera->Render();

	//Get the world, view, and projection matrices from the opengl and camera objects.
	gl.GetWorldMatrix(worldMatrix);
	gl.GetProjectionMatrix(projectionMatrix);
}
void Scene::Render(OpenGL& gl) {
	Matrix<float, 4, 4> worldMatrix;
	Matrix<float, 4, 4> viewMatrix;
	Matrix<float, 4, 4> projectionMatrix;

	worldMatrix = GetWorldMatrix();
	viewMatrix = GetViewMatrix();
	projectionMatrix = GetProjectionMatrix();
	
	float diffuseLightColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	float lightDirection[3] = { 0.0f, 0.0f, 0.0f };
	diffuseLight.GetDiffuseColor(diffuseLightColor);
	diffuseLight.GetDirection(lightDirection);

	pShader->SetShader(gl);
	pShader->SetShaderParameters(gl, (float*)worldMatrix.value, (float*)viewMatrix.value, (float*)projectionMatrix.value, 0, lightDirection, diffuseLightColor);
	pShader->Render(gl);
}

void Scene::Shutdown(OpenGL& gl) {
	if (pShader)
		pShader->Shutdown(gl);
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
