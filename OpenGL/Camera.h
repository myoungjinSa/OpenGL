#pragma once
#include "Common.h"
#include "Matrix.h"

class Camera
{
public:
	Camera();
	Camera(const Camera& other) = delete;
	~Camera();

	void SetPosition(float x, float y, float z);
	Vec3f GetPosition() const;
	void GetPosition(std::array<float, 4>& position);

	void SetRotation(float x, float y, float z);
	Vec3f GetRotation() const;

	void SetDirection(const Vec3f& lookVector);
	void GetDirection(Vec3f& lookVector);

	void Render();
	void GetViewMatrix(Matrix<float, 4, 4>& Matrix) const;
private:
	void MatrixRotationYawPitchRoll(Matrix<float, 3, 3>& Matrix, float, float, float);
	void TransformCoord(Vec3f& Vec, Matrix<float, 3, 3>& Matrix);
	void BuildViewMatrix(Vec3f, Vec3f, Vec3f);
	
	Vec3f position;
	Vec3f rotation;
	Vec3f direction;
	Matrix<float, 4, 4> viewMatrix;
};

