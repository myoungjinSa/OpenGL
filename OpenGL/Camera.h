#pragma once

#include<math.h>
#include "Matrix.h"

class Camera
{
public:
	Camera();
	Camera(const Camera& other) = delete;
	~Camera();

	void SetPosition(float x, float y, float z);
	void SetRotation(float x, float y, float z);

	void Render();
	void GetViewMatrix(float* pMat);
private:
	void MatrixRotationYawPitchRoll(float* pMat, float, float, float);
	void TransformCoord(Vec3f& Vec, float* pMat);
	void BuildViewMatrix(Vec3f, Vec3f, Vec3f);
	
	Vec3f position;
	Vec3f rotation;
	Matrix<float, 4, 4> viewMatrix;
};

