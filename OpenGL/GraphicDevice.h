#pragma once
#include "Platform.h"

class GraphicDevice {
public:
	virtual ~GraphicDevice() {}

	virtual bool InitializeExtensions(HWND hWnd) { return true; }
	virtual bool Initialize(HWND hWnd, int screenWidth, int screenHeight, bool vSync) = 0;
	virtual void Shutdown(HWND hWnd) = 0;

	virtual void BeginScene(float red, float green, float blue, float alpha) = 0;
	virtual void EndScene() = 0;
	virtual void GetVideoCardInfo(char* videoCardList) = 0;

	virtual unsigned int GetScreenWidth()const = 0;
	virtual unsigned int GetScreenHeight()const = 0;

	virtual char* GetShaderLog(unsigned int programID) { return nullptr; }
	virtual char* GetLinkerLog(unsigned int programID) { return nullptr; }

};




