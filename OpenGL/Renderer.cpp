#include "OpenGL.h"
#include "Camera.h"
#include "Shader.h"
#include "Object.h"
#include "Renderer.h"


Renderer::Renderer() 
	:pGL(nullptr), 
	pCamera(nullptr),
	pObject(nullptr),
	pShader(nullptr)
{
}

Renderer::Renderer(const Renderer& other) {

}

Renderer::~Renderer() {
	if (pCamera) {
		delete pCamera;
		pCamera = nullptr;
	}
	if (pObject){
		delete pObject;
		pObject = nullptr;
	}
	if (pShader) {
		delete pShader;
		pShader = nullptr;
	}
}

bool Renderer::Initialize(OpenGL* _pOpenGL, HWND hWnd) {
	
	//Store a pointer to ther OpenGL class object.
	pGL = _pOpenGL;

	pCamera = new Camera();
	pCamera->SetPosition(0.0f, 0.0f, -10.0f);

	pObject = new Object();
	if (!pObject)
		return false;


	bool result = pObject->Initialize(pGL);
	if (!result) {
		MessageBox(hWnd, L"Could not initialize the model object", L"Error", MB_OK);
		return false;
	}

	pShader = new ColorShader();
	if (!pShader)
		return false;

	result = pShader->Initialize(pGL, hWnd);
	if (!result) {
		MessageBox(hWnd, L"Could not initialize the color shader object.", L"Error", MB_OK);
		return false;
	}

	return true;
}

void Renderer::Shutdown() {
	pGL = 0;
}


bool Renderer::Frame()
{
	bool result;


	// Render the graphics scene.
 	result = Render();
	if (!result)
	{
		return false;
	}

	return true;
}


bool Renderer::Render()
{
	Matrix<float, 4, 4> worldMatrix;
	Matrix<float, 4, 4> viewMatrix;
	Matrix<float, 4, 4> projectionMatrix;

	// Clear the buffers to begin the scene.
	pGL->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	pCamera->Render();

	//Get the world, view, and projection matrices from the opengl and camera objects.
	pGL->GetWorldMatrix(worldMatrix);
	pCamera->GetViewMatrix(viewMatrix);
	pGL->GetProjectionMatrix(projectionMatrix);

	pShader->SetShader(pGL);
	pShader->SetShaderParameters(pGL, (float*)worldMatrix.value, (float*)viewMatrix.value, (float*)projectionMatrix.value);
	
	pObject->Render(pGL);

	// Present the rendered scene to the screen.
	pGL->EndScene();
	return true;
}


