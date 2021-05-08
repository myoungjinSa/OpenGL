#pragma once
#include "OpenGL.h"
/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class OpenGL;
class Object;
class PhongShader;
class ColorShader;
class Camera;
class Scene;
class Renderer
{
public:
	enum class DrawMode : unsigned int {
		QUADS,
		QUAD_STRIP,
		POINTS,
		LINES,
		LINE_LOOP,
		POLYGON,
		TRIANGLES,
		NUM_DRAW_MODES
	};

	Renderer();
	Renderer(const Renderer&) = delete;
	~Renderer();

	bool Initialize(OpenGL* pOpenGL, HWND hWnd); 
	void Shutdown();
	bool Frame();

	void Attach(Scene& scene);
	void Detach();
private:
	bool Render(Scene& scene);
private:
	OpenGL* pGL;
	//PhongShader* pShader;
	Scene* pTargetScene;
};

