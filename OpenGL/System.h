#pragma once
#include <Windows.h>


class OpenGL;
class Input;
class Renderer;
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
};


static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
/////////
//GLOBALS
/////////
static System* applicationHandle = 0;