#pragma once
#include <Windows.h>
#include <clocale>
#include "Timer.h"
//#include "GL/glut.h"
#include "KeyCode.h"

//#pragma comment(lib,"freeglut.lib")

#define GET_X_LPARAM(lp)	((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp)	((int)(short)HIWORD(lp))

inline bool IsKeyDown(int key) {
	return (GetAsyncKeyState(key) & 0x8000) != 0;
}

class GraphicDevice;
class Input;
class Renderer;
class Scene;
class SceneEdit;
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
	void ProcessInput();
	bool Frame(float elapsedTime);
	bool InitializeWindows(GraphicDevice* pGraphicDevice, int&, int&);
	void ShutdownWindows();

private:
	LPCWSTR applicationName;
	HINSTANCE hInstance;
	HWND hWnd;

	int screenWidth;
	int screenHeight;

	GraphicDevice* pGraphicDevice;
	Input* pInput;
	Renderer* pRenderer;
	Scene* pScene;
	SceneEdit* pSceneEdit;
	Timer timer;
};


static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
/////////
//GLOBALS
/////////
static System* applicationHandle = 0;