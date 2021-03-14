#include "OpenGL.h"
#include "Renderer.h"


Renderer::Renderer() {
	pOpenGL = 0;
}

Renderer::Renderer(const Renderer& other) {

}

Renderer::~Renderer() {

}

bool Renderer::Initialize(OpenGL* _pOpenGL, HWND hWnd) {
	
	//Store a pointer to ther OpenGL class object.
	pOpenGL = _pOpenGL;
	return true;
}

void Renderer::Shutdown() {
	pOpenGL = 0;
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
	// Clear the buffers to begin the scene.
	pOpenGL->BeginScene(1.0f, 0.5f, 0.5f, 1.0f);


	// Present the rendered scene to the screen.
	pOpenGL->EndScene();
	return true;
}


