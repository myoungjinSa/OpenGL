#pragma once
#include "OpenGL.h"

class ColorShader 
{
public:
	ColorShader();
	ColorShader(const ColorShader& other) = delete;
	~ColorShader();

	bool Initialize(OpenGL* pGL, HWND hWnd);
	void Shutdown(OpenGL* pGL);
	void SetShader(OpenGL* pGL);

	bool SetShaderParameters(OpenGL* pGL, float* worldMatrix, float* viewMatrix, float* projectionMatrix);

private:
	bool InitializeShader(const char* vsFilename,const char* fsFilename, OpenGL* pGL, HWND hWnd);
	char* LoadShaderSourceFile(const char* filename);
	void OutputShaderErrorMessage(OpenGL* pGL, HWND hWnd, unsigned int shaderId, char*);
	void OutputLinkErrorMessage(OpenGL* pGL, HWND hWnd, unsigned int programId);
	void ShutdownShader(OpenGL* pGL);
	
	unsigned int vertexShader;
	unsigned int fragmentShader;
	unsigned int shaderProgram;
};