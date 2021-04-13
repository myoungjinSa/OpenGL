#pragma once
#include "OpenGL.h"

class OpenGL;
class Shader {
public:
	Shader();
	Shader(const Shader& other) = delete;
	virtual ~Shader();

	virtual bool Initialize(OpenGL* pGL, HWND hWnd) = 0;
	virtual void Shutdown(OpenGL* pGL) = 0;
	virtual void SetShader(OpenGL* pGL) = 0;
protected:
	virtual bool InitializeShader(const char* vsFilename, const char* fsFilename, OpenGL* pGL, HWND hWnd) = 0;

	void OutputShaderErrorMessage(OpenGL* pGL, HWND hWnd, unsigned int shaderId, char*);
	void OutputLinkErrorMessage(OpenGL* pGL, HWND hWnd, unsigned int programId);
	char* LoadShaderSourceFile(const char* filename);
	void ShutdownShader(OpenGL* pGL);
	bool CompileShader(const char* vsFilename, const char* fsFilename, OpenGL* pGL, HWND hWnd);

	unsigned int vertexShader;
	unsigned int fragmentShader;
	unsigned int shaderProgram;
};


class ColorShader : public Shader
{
public:
	ColorShader();
	ColorShader(const ColorShader& other) = delete;
	~ColorShader() override;

	bool Initialize(OpenGL* pGL, HWND hWnd) override;
	void Shutdown(OpenGL* pGL) override;
	void SetShader(OpenGL* pGL) override;


	bool SetShaderParameters(OpenGL* pGL, float* worldMatrix, float* viewMatrix, float* projectionMatrix);
protected:
	bool InitializeShader(const char* vsFilename, const char* fsFilename, OpenGL* pGL, HWND hWnd);
};

class TextureShader : public Shader 
{
public:
	TextureShader();
	TextureShader(const TextureShader& other) = delete;
	~TextureShader() override;

	bool Initialize(OpenGL* pGL, HWND hWnd) override;
	void Shutdown(OpenGL* pGL) override;
	void SetShader(OpenGL* pGL) override;

	bool SetShaderParameters(OpenGL* pGL, float* worldMatrix, float* viewMatrix, float* projectionMatrix, int textureUnit);
protected:
	bool InitializeShader(const char* vsFilename, const char* fsFilename, OpenGL* pGL, HWND hWnd);
};