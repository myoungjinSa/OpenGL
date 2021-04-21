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
class TextureShader;
class ColorShader;
class Camera;
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
	//ColorShader* pShader;
	TextureShader* pShader;
};

