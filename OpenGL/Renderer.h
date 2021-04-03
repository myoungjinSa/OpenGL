#pragma once

/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class OpenGL;
class Object;
class ColorShader;
class Camera;
class Renderer
{
public:
	Renderer();
	Renderer(const Renderer&);
	~Renderer();

	bool Initialize(OpenGL* pOpenGL, HWND hWnd);
	void Shutdown();
	bool Frame();
	
private:
	bool Render();
private:
	OpenGL* pGL;
	Camera* pCamera;
	Object* pObject;
	ColorShader* pShader;
};

