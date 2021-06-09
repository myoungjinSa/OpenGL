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
	virtual void Render(Renderer& renderer) = 0;
	virtual void Render(Renderer& renderer, Matrix<float, 4, 4>& viewMatrix, Matrix<float, 4, 4>& projectionMatrix, Vec3f& lightPosition, Vec3f& lightDirection) = 0;
	void SetShader(Renderer& renderer);
protected:
	virtual bool InitializeShader(const char* vsFilename, const char* fsFilename, Renderer& renderer, HWND hWnd) = 0;
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
	void Render(Renderer& renderer) override;
	void Render(Renderer& renderer, Matrix<float, 4, 4>& viewMatrix, Matrix<float, 4, 4>& projectionMatrix, Vec3f& lightPosition, Vec3f& lightDirection) override {}
	bool SetShaderParameters(Renderer& renderer, Matrix<float, 4, 4>& worldMatrix, Matrix<float, 4, 4>& viewMatrix, Matrix<float, 4, 4>& projectionMatrix);
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
	void Render(Renderer& renderer) override {};
	void Render(Renderer& gl, Matrix<float, 4, 4>& viewMatrix, Matrix<float, 4, 4>& projectionMatrix, Vec3f& lightPosition, Vec3f& lightDirection) override;
	
	bool SetShaderParameters(Renderer& renderer, Matrix<float, 4, 4>& viewMatrix, Matrix<float, 4, 4>& projectionMatrix, Vec3f& lightDirection, int objectIndex);
protected:
	bool InitializeShader(const char* vsFilename, const char* fsFilename, Renderer& renderer, HWND hWnd);
};


