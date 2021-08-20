#pragma once
#include <vector>
#include "Light.h"
#include "Matrix.h"
#include "Object.h"

class Shader;
class ShaderParameter;

class Camera;
class Renderer;
class Ray;
class Material;
class Scene final : public Object
{
public:
	Scene();
	~Scene();

	bool BuildObject(Renderer& gl);
	void Update(const float& elapsedTime);
	bool Render(Renderer& renderer);
	void Shutdown(Renderer& gl);

	Matrix<float, 4, 4> GetViewMatrix() const;
	Matrix<float, 4, 4> GetProjectionMatrix() const;

	size_t	GetObjectCount() const;
	void	Picking(int x, int y, int screenWidth, int screenHeight);
	bool	IntersectObjects(const Ray& ray) const;

	void	FillShaderParameter(const GameObject& gameObject, ShaderParameter& shaderParam);
private:
	Camera* pCamera;
	
	std::shared_ptr<Shader> DefaultShader;
	std::vector<GameObject*> objects;
	Light phongLight;

	Matrix<float, 4, 4> projectionMatrix;
};

