#include "Renderer.h"
#include "Camera.h"
#include "Scene.h"
#include "Shader.h"
#include "Object.h"
#include "Texture.h"

Renderer::Renderer() 
	:pGL(nullptr),
	pTargetScene(nullptr)
{
}

Renderer::~Renderer() {
	pTargetScene = nullptr;
}

bool Renderer::Initialize(OpenGL* pOpenGL, HWND hWnd) {
	//Store a pointer to ther OpenGL class object.
	pGL = pOpenGL;
	if (!pGL)
		return false;
	
	return true;
}
void Renderer::Shutdown() {

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

bool Renderer::Render(Scene& targetScene) {

	// Clear the buffers to begin the scene.
	pGL->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);
	targetScene.Prepare(*pGL);

	targetScene.Render(*pGL);

	// Present the rendered scene to the screen.
	pGL->EndScene();
	return true;
}


