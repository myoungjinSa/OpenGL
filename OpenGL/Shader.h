#pragma once
#include "Common.h"
#include "OpenGL.h"

class OpenGL;
class Object;
class ShaderVisitor;
class Renderer;
class Ray;

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

	virtual bool Initialize(Renderer& renderer) = 0;
	virtual void Shutdown(Renderer& renderer);
	virtual void Update(float elapsedTime) = 0;
	virtual void Render(Renderer& renderer, Matrix<float, 4, 4>& viewMatrix, Matrix<float, 4, 4>& projectionMatrix) = 0;
	virtual void Render(Renderer& renderer, Matrix<float, 4, 4>& viewMatrix, Matrix<float, 4, 4>& projectionMatrix, Vec3f& lightPosition, Vec3f& cameraPosition) = 0;
	void SetShader(Renderer& renderer);

	size_t GetObjectCount() const;
	
	bool IntersectObjects(const Ray& ray) const;
protected:
	virtual bool InitializeShader(const char* vsFilename, const char* fsFilename, Renderer& renderer) = 0;
	void ShutdownShader(Renderer& gl);

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

	bool Initialize(Renderer& renderer) override;
	void Shutdown(Renderer& renderer) override;
	void Update(float elapsedTime) override;
	void Render(Renderer& renderer, Matrix<float, 4, 4>& viewMatrix, Matrix<float, 4, 4>& projectionMatrix) override;
	void Render(Renderer& renderer, Matrix<float, 4, 4>& viewMatrix, Matrix<float, 4, 4>& projectionMatrix, Vec3f& lightPosition, Vec3f& lightDirection) override {}
	bool SetShaderParameters(Renderer& renderer, Matrix<float, 4, 4>& viewMatrix, Matrix<float, 4, 4>& projectionMatrix, int objectIndex);
	//bool SetShaderParameters(Renderer& renderer, float* worldMatrix, float* viewMatrix, float* projectionMatrix);
protected:
	bool InitializeShader(const char* vsFilename, const char* fsFilename, Renderer& renderer);
};

class PhongShader : public Shader 
{
public:
	PhongShader();
	PhongShader(const PhongShader& other) = delete;
	~PhongShader() override;

	bool Initialize(Renderer& renderer) override;
	void Shutdown(Renderer& renderer) override;
	void Update(float elapsedTime) override;
	void Render(Renderer& renderer, Matrix<float, 4, 4>& viewMatrix, Matrix<float, 4, 4>& projectionMatrix) override {};
	void Render(Renderer& gl, Matrix<float, 4, 4>& viewMatrix, Matrix<float, 4, 4>& projectionMatrix, Vec3f& lightPosition, Vec3f& cameraPosition) override;
	
	bool SetShaderParameters(Renderer& renderer, Matrix<float, 4, 4>& viewMatrix, Matrix<float, 4, 4>& projectionMatrix, Vec3f& lightPosition, Vec3f& cameraPosition, int objectIndex);
protected:
	bool InitializeShader(const char* vsFilename, const char* fsFilename, Renderer& renderer);
};

//class WireframeShader : public Shader {
//public:
//	WireframeShader();
//	WireframeShader(const WireframeShader& other) = delete;
//	~WireframeShader() override;
//
//	bool Initialize(Renderer& renderer) override;
//	void Shutdown(Renderer& renderer) override;
//
//
//};