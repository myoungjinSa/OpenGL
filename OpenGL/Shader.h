#pragma once
#include "Common.h"
#include "OpenGL.h"

class OpenGL;
class Object;
class GameObject;
class ShaderVisitor;
class Renderer;
class Ray;
class BoundingVolume;

struct ShaderParameter {
	Matrix<float, 4, 4> worldMatrix;
	Matrix<float, 4, 4> viewMatrix;
	Matrix<float, 4, 4> projectionMatrix;

	Vec3f lightPosition;
	Vec4f diffuseAlbedo;
	Vec4f specularAlbedo;
	Vec4f ambientAlbedo;

	Vec3f cameraPosition;

	int textureUnit;
};

class Shader {
public:
	Shader();
	Shader(const Shader& other) = delete;
	virtual ~Shader();

	virtual bool Initialize(Renderer& renderer) = 0;
	virtual void Shutdown(Renderer& renderer);
	virtual void Update(float elapsedTime) = 0;
	void SetShader(Renderer& renderer);

	virtual void Render(Renderer& renderer, ShaderParameter& shaderParam) = 0;
	
	virtual bool SetShaderParameters(Renderer& renderer, ShaderParameter& shaderParam) = 0;
protected:
	virtual bool InitializeShader(const char* vsFilename, const char* fsFilename, Renderer& renderer) = 0;
	void ShutdownShader(Renderer& gl);

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

	bool Initialize(Renderer& renderer) override;
	void Shutdown(Renderer& renderer) override;
	void Update(float elapsedTime) override;
	
	void Render(Renderer& renderer, ShaderParameter& shaderParam) override;
	virtual bool SetShaderParameters(Renderer& renderer, ShaderParameter& shaderParam);
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
	void Render(Renderer& renderer, ShaderParameter& shaderParam) override;
	
	virtual bool SetShaderParameters(Renderer& renderer, ShaderParameter& shaderParam);
protected:
	bool InitializeShader(const char* vsFilename, const char* fsFilename, Renderer& renderer);
};
