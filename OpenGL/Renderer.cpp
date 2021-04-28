#include "Renderer.h"
#include "Camera.h"
#include "Scene.h"
#include "Shader.h"
#include "Object.h"
#include "Texture.h"

Renderer::Renderer() 
	:pGL(nullptr),
	pShader(nullptr),
	pTargetScene(nullptr)
{
}

Renderer::~Renderer() {
	if (pShader) {
		delete pShader;
		pShader = nullptr;
	}
	pTargetScene = nullptr;
}

bool Renderer::Initialize(OpenGL* pOpenGL, HWND hWnd) {
	//Store a pointer to ther OpenGL class object.
	pGL = pOpenGL;
	if (!pGL)
		return false;
	
	pShader = new TextureShader();
	if (!pShader)
		return false;

	bool result = pShader->Initialize(*pGL, hWnd);
	if (!result) {
		MessageBox(hWnd, L"Could not initialize the color shader object.", L"Error", MB_OK);
		return false;
	}
	return true;
}
void Renderer::Shutdown() {
	if(pShader)
		pShader->Shutdown(*pGL);

	Detach();
	TextureLoader::Release();
}


bool Renderer::Frame()
{
	bool result;

	// Render the graphics scene.
 	result = Render(*pTargetScene);
	if (!result)
	{
		return false;
	}

	return true;
}

void Renderer::Attach(Scene& targetScene) {
	pTargetScene = &targetScene;
}

void Renderer::Detach() {
	pTargetScene = nullptr;
}

bool Renderer::Render(Scene& targetScene)
{
	Matrix<float, 4, 4> worldMatrix;
	Matrix<float, 4, 4> viewMatrix;
	Matrix<float, 4, 4> projectionMatrix;

	// Clear the buffers to begin the scene.
	pGL->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);
	targetScene.Prepare(*pGL);

	worldMatrix = targetScene.GetWorldMatrix();
	viewMatrix = targetScene.GetViewMatrix();
	projectionMatrix = targetScene.GetProjectionMatrix();

	pShader->SetShader(*pGL);
	pShader->SetShaderParameters(*pGL, (float*)worldMatrix.value, (float*)viewMatrix.value, (float*)projectionMatrix.value, 0);
	
	targetScene.Render(*pGL);

	// Present the rendered scene to the screen.
	pGL->EndScene();
	return true;
}


