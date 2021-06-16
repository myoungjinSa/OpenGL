#pragma once
#include <vector>
#include "Light.h"
#include "Matrix.h"

class ColorShader;
class PhongShader;
class Object;
class Camera;
class Renderer;
class Scene final
{
public:
	Scene();
	~Scene();

	bool BuildObject(Renderer& gl, HWND hWnd);
	void Update(const float& elapsedTime);
	bool Render(Renderer& renderer);
	void Shutdown(Renderer& gl);

	Matrix<float, 4, 4> GetWorldMatrix() const;
	Matrix<float, 4, 4> GetViewMatrix() const;
	Matrix<float, 4, 4> GetProjectionMatrix() const;
private:
	Camera* pCamera;
	PhongShader* pShader;
	Light phongLight;



	Matrix<float, 4, 4> worldMatrix;
	Matrix<float, 4, 4> projectionMatrix;
};

