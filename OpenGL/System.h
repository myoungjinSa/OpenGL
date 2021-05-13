#pragma once
#include <Windows.h>
#include "GL/glut.h"

#pragma comment(lib,"freeglut.lib")

#define GET_X_LPARAM(lp)	((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp)	((int)(short)HIWORD(lp))

class OpenGL;
class Input;
class Renderer;
class Scene;
class System
{
public:
	System();
	System(const System& other);
	~System();

	bool Initialize();
	void Shutdown();
	void Run();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	bool Frame();
	bool InitializeWindows(OpenGL* gl, int&, int&);
	void ShutdownWindows();

private:
	LPCWSTR applicationName;
	HINSTANCE hInstance;
	HWND hWnd;

	OpenGL* pOpenGL;
	Input* pInput;
	Renderer* pRenderer;
	Scene* pScene;
};


static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
/////////
//GLOBALS
/////////
static System* applicationHandle = 0;