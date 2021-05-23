#pragma once
#include "Common.h"
#include "OpenGL.h"

class OpenGL;
class Object;
class ShaderVisitor;
class Renderer;
struct ShaderParameter {
	Matrix<float, 4, 4> worldMatrix;
	Matrix<float, 4, 4> viewMatrix;
	Matrix<float, 4, 4> projectionMatrix;

	std::array<float, 3> lightDirection;
	std::array<float, 4> diffuseAlbedo;
	std::array<float, 4> specularAlbedo;
	std::array<float, 4> ambientAlbedo;

	int textureUnit;
};

typedef void(ShaderParameterBindCallback)(const ShaderParameter& param);

class Shader {
public:
	Shader();
	Shader(const Shader& other) = delete;
	virtual ~Shader();

	virtual bool Initialize(Renderer& renderer, HWND hWnd) = 0;
	virtual void Shutdown(Renderer& renderer);
	virtual void SetShader(Renderer& renderer);
	virtual void Render(Renderer& renderer) = 0;
protected:
	virtual bool InitializeShader(const char* vsFilename, const char* fsFilename, OpenGL& gl, HWND hWnd) = 0;
	void ShutdownShader(Renderer& gl);

	/*void OutputShaderErrorMessage(OpenGL& gl, HWND hWnd, unsigned int shaderId, char*);
	void OutputLinkErrorMessage(OpenGL& gl, HWND hWnd, unsigned int programId);
	char* LoadShaderSourceFile(const char* filename);
	
	bool CompileShader(const char* vsFilename, const char* fsFilename, OpenGL& pGL, HWND hWnd);*/

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

	bool Initialize(Renderer& renderer, HWND hWnd) override;
	void Shutdown(Renderer& renderer) override;
	void SetShader(Renderer& renderer) override;
	void Render(Renderer& renderer) override;

	bool SetShaderParameters(Renderer& renderer, std::array<std::array<float, 4>, 4>& worldMatrix, std::array<std::array<float, 4>, 4>& viewMatrix, std::array<std::array<float, 4>, 4>& projectionMatrix);
	//bool SetShaderParameters(Renderer& renderer, float* worldMatrix, float* viewMatrix, float* projectionMatrix);
protected:
	bool InitializeShader(const char* vsFilename, const char* fsFilename, Renderer& renderer, HWND hWnd);
};

class PhongShader : public Shader 
{
public:
	PhongShader();
	PhongShader(const PhongShader& other) = delete;
	~PhongShader() override;

	bool Initialize(Renderer& renderer, HWND hWnd) override;
	void Shutdown(Renderer& renderer) override;
	void SetShader(Renderer& renderer) override;
	void Render(Renderer& gl) override;
	
	bool SetShaderParameters(Renderer& renderer, std::array<std::array<float, 4>, 4>& worldMatrix, std::array<std::array<float, 4>, 4>& viewMatrix, std::array<std::array<float, 4>, 4>& projectionMatrix 
		, std::array<std::array<float, 3>, 3>& lightDirection, std::array<std::array<float, 3>, 3>& diffuseAlbedo, std::array<std::array<float, 4>, 4>& ambientAlbedo, std::array<std::array<float, 3>, 3>& specular, int textureUnit);
	/*bool SetShaderParameters(Renderer& renderer, float* worldMatrix, float* viewMatrix, float* projectionMatrix, int textureUnit, 
		float* lightDirection, float* diffuseAlbedo, float* ambientAlbedo, float* specularAlbedo);*/
protected:
	bool InitializeShader(const char* vsFilename, const char* fsFilename, Renderer& renderer, HWND hWnd);
};


