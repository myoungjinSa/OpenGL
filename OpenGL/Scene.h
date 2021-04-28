#pragma once
#include <vector>
#include "Matrix.h"
class Object;
class Camera;
class Scene final
{
public:
	Scene();
	~Scene();

	bool BuildObject(OpenGL& gl, HWND hWnd);
	void Prepare(OpenGL& gl);
	void Render(OpenGL& gl);
	void Shutdown(OpenGL& gl);


	const Matrix<float, 4, 4>& GetWorldMatrix() const;
	const Matrix<float, 4, 4>& GetViewMatrix() const;
	const Matrix<float, 4, 4>& GetProjectionMatrix() const;
private:
	Camera* pCamera;
	std::vector<Object*> objects;

	Matrix<float, 4, 4> worldMatrix;
	Matrix<float, 4, 4> projectionMatrix;
};

