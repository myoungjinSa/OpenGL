#include "Shader.h"
#include "Camera.h"
#include "Scene.h"
#include "Object.h"

Scene::Scene() 
	:pCamera(nullptr),
	 worldMatrix(),
	 projectionMatrix()
{
	objects.clear();
}

Scene::~Scene() {
	if (pCamera) {
		delete pCamera;
		pCamera = nullptr;
	}
	for (auto& obj : objects) {
		delete obj;
	}
	objects.clear();
}

bool Scene::BuildObject(OpenGL& gl, HWND hWnd) {
	pCamera = new Camera();
	pCamera->SetPosition(5.0f, 3.0f, -10.0f);

	Object* pObject = new Object();
	objects.emplace_back(pObject);

	for (const auto& obj : objects) {
		if (!obj->Initialize(gl)) {
			MessageBox(hWnd, L"Could not initialize the model object", L"Error", MB_OK);
			return false;
		}
	}
}
void Scene::Prepare(OpenGL& gl) {
	pCamera->Render();

	//Get the world, view, and projection matrices from the opengl and camera objects.
	gl.GetWorldMatrix(worldMatrix);
	gl.GetProjectionMatrix(projectionMatrix);
}
void Scene::Render(OpenGL& gl) {
	for (const auto& obj : objects)
		obj->Render(gl);
}

void Scene::Shutdown(OpenGL& gl) {
	for (const auto& obj : objects)
		obj->Shutdown(gl);
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
