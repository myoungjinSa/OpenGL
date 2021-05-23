#pragma once
#include <vector>
#include "Light.h"
#include "Matrix.h"

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
	void Prepare(Renderer& gl);
	void Update(const float& elapsedTime);
	bool Render(Renderer& renderer);
	void Shutdown(Renderer& gl);

	const Matrix<float, 4, 4>& GetWorldMatrix() const;
	const Matrix<float, 4, 4>& GetViewMatrix() const;
	const Matrix<float, 4, 4>& GetProjectionMatrix() const;
private:
	Camera* pCamera;
	PhongShader* pShader;
	Light phongLight;

	Matrix<float, 4, 4> worldMatrix;
	Matrix<float, 4, 4> projectionMatrix;
};

