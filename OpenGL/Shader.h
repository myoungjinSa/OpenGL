#pragma once
#include "OpenGL.h"
#include <vector>

class OpenGL;
class Object;
class ShaderVisitor;

class Shader {
public:
	Shader();
	Shader(const Shader& other) = delete;
	virtual ~Shader();

	virtual bool Initialize(OpenGL& gl, HWND hWnd) = 0;
	virtual void Shutdown(OpenGL& gl);
	virtual void SetShader(OpenGL& gl);
	virtual void Render(OpenGL& gl) = 0;
protected:
	virtual bool InitializeShader(const char* vsFilename, const char* fsFilename, OpenGL& gl, HWND hWnd) = 0;


	void OutputShaderErrorMessage(OpenGL& gl, HWND hWnd, unsigned int shaderId, char*);
	void OutputLinkErrorMessage(OpenGL& gl, HWND hWnd, unsigned int programId);
	char* LoadShaderSourceFile(const char* filename);
	void ShutdownShader(OpenGL& gl);
	bool CompileShader(const char* vsFilename, const char* fsFilename, OpenGL& pGL, HWND hWnd);

	unsigned int vertexShader;
	unsigned int fragmentShader;
	unsigned int shaderProgram;
	std::vector<Object*> objects;
};


class ColorShader : public Shader
{
public:
	ColorShader();
	ColorShader(const ColorShader& other) = delete;
	~ColorShader() override;

	bool Initialize(OpenGL& gl, HWND hWnd) override;
	void Shutdown(OpenGL& gl) override;
	void SetShader(OpenGL& gl) override;
	void Render(OpenGL& gl) override;

	bool SetShaderParameters(OpenGL& gl, float* worldMatrix, float* viewMatrix, float* projectionMatrix);
protected:
	bool InitializeShader(const char* vsFilename, const char* fsFilename, OpenGL& gl, HWND hWnd);
};

class TextureShader : public Shader 
{
public:
	TextureShader();
	TextureShader(const TextureShader& other) = delete;
	~TextureShader() override;

	bool Initialize(OpenGL& gl, HWND hWnd) override;
	void Shutdown(OpenGL& gl) override;
	void SetShader(OpenGL& gl) override;
	void Render(OpenGL& gl) override;
	
	bool SetShaderParameters(OpenGL& pGL, float* worldMatrix, float* viewMatrix, float* projectionMatrix, int textureUnit, 
		float* lightDirection, float* diffuseLightColor, float* ambientLight);
protected:
	bool InitializeShader(const char* vsFilename, const char* fsFilename, OpenGL& gl, HWND hWnd);
};


