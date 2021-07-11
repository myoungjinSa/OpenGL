#pragma once
#include <array>
#include "Matrix.h"
#include "Component.h"

class Object;
class Transform : public Component 
{
public:
	Transform(Object* owner);

	void AddPosition(const Vec3f& position);
	void AddPosition(float x, float y, float z);
	void SetPosition(const Vec3f& position);
	void SetPosition(float x, float y, float z);
	Vec3f GetPosition() const;
	Vec3f GetRotation() const;
	void GetPosition(std::array<float, 4>& position);
	Matrix<float, 3, 3> GetRotationMatrix() const;

	Vec4f Rotate(const Vec4f& pos, const Vec3f& pivot, float pitch, float yaw, float roll);
	Vec4f Rotate(const Vec3f& pos, const Vec3f& pivot, float pitch, float yaw, float roll);
	void Rotate(float pitch, float yaw, float roll);
	void Move(const Vec3f& direction, float elapsedTime);

	void SetLook(const Vec3f& look);
	void SetLook(Vec3f&& look) noexcept;

	void SetUp(const Vec3f& up);
	void SetUp(Vec3f&& up) noexcept;

	void SetRight(const Vec3f& right);
	void SetRight(Vec3f&& right) noexcept;

	Vec3f GetLook() const;
	Vec3f GetRight() const;
	Vec3f GetUp() const;

	float GetMovingSpeed() const;
private:
	void CalculateRotationMatrix(Matrix<float, 4, 4>& rotationMatrix, float pitch, float yaw, float roll) const;

private:
	Vec3f position;
	Vec3f rotation;

	Matrix<float, 4, 4> worldMatrix;

	float movingSpeed;
};