#pragma once
#include "Common.h"
#include "OpenGL.h"
#include "Component.h"

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

class Shader : public Component{
public:
	Shader(Object* _pOwner);
	Shader(const Shader& other) = delete;
	virtual ~Shader();

	virtual bool Initialize(Renderer& renderer) = 0;
	virtual void Shutdown(Renderer& renderer);
	void SetShader(Renderer& renderer);

	virtual void Render(Renderer& renderer, const ShaderParameter& shaderParam) = 0;
	
	virtual bool SetShaderParameters(Renderer& renderer, const ShaderParameter& shaderParam) = 0;
protected:
	virtual bool InitializeShader(const char* vsFilename, const char* fsFilename, Renderer& renderer) = 0;
	
	unsigned int vertexShader;
	unsigned int fragmentShader;
	unsigned int shaderProgram;
};


class ColorShader : public Shader
{
public:
	ColorShader(Object* pOwner);
	ColorShader(const ColorShader& other) = delete;
	~ColorShader() override;

	bool Initialize(Renderer& renderer) override;
	void Shutdown(Renderer& renderer) override;
	
	void Render(Renderer& renderer, const ShaderParameter& shaderParam) override;
	bool SetShaderParameters(Renderer& renderer, const ShaderParameter& shaderParam) override;
protected:
	bool InitializeShader(const char* vsFilename, const char* fsFilename, Renderer& renderer);
};

class TextureShader : public Shader {
public:
	TextureShader(Object* pOwner);
	TextureShader(const TextureShader& other) = delete;
	~TextureShader() override;

	bool Initialize(Renderer& renderer) override;
	void Shutdown(Renderer& renderer) override;
	void Render(Renderer& renderer, const ShaderParameter& shaderParam) override;
	
	bool SetShaderParameters(Renderer& renderer, const ShaderParameter& shaderParam) override;
protected:
	bool InitializeShader(const char* vsFilename, const char* fsFilename, Renderer& renderer);
};

class PhongShader : public Shader 
{
public:
	PhongShader(Object* pOwner);
	PhongShader(const PhongShader& other) = delete;
	~PhongShader() override;

	bool Initialize(Renderer& renderer) override;
	void Shutdown(Renderer& renderer) override;
	void Render(Renderer& renderer, const ShaderParameter& shaderParam) override;
	
	bool SetShaderParameters(Renderer& renderer, const ShaderParameter& shaderParam) override;
protected:
	bool InitializeShader(const char* vsFilename, const char* fsFilename, Renderer& renderer);
};
