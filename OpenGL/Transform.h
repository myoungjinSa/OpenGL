#pragma once
#include <array>
#include "Matrix.h"
#include "Component.h"

class Object;
class RigidTransform : public Component 
{
public:
	RigidTransform(Object* owner);

	Vec3f TransformVector(const Vec3f& vec)const;
	Vec3f TransformPoint(const Vec3f& point)const;
	Vec3f DetransformVector(const Vec3f& vec)const;
	Vec3f DetransformPoint(const Vec3f& point)const;

	void AddPosition(const Vec3f& position);
	void AddPosition(float x, float y, float z);
	void SetPosition(const Vec3f& position);
	void SetPosition(float x, float y, float z);
	Vec3f GetPosition() const;
	void GetPosition(std::array<float, 4>& position);
	Matrix<float, 3, 3> GetRotationMatrix() const;


	//Vec4f Rotate(const Vec4f& pos, const Vec3f& pivot, float pitch, float yaw, float roll);
	Vec4f Rotate(const Vec3f& pos, const Vec3f& pivot, float pitch, float yaw, float roll);
	void Rotate(float pitch, float yaw, float roll);
	
	void SetScale(float sx, float sy, float sz);
	void SetScale(const Vec3f& size);
	Vec3f GetScale() const;

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

	void  SetMovingSpeed(float speed);
	float GetMovingSpeed() const;

	const Matrix<float, 4, 4>& GetWorldMatrix() const;
	void SetWorldMatrix(const Matrix<float, 4, 4>& worldMatrix);
private:
	void CalculateRotationMatrix(Matrix<float, 4, 4>& rotationMatrix, float pitch, float yaw, float roll, bool bUseQuaternion = true) const;
	
	void SetTransform();
private:
	Vec3f position;
	Vec4d orientation;
	Vec3f scale;

	Matrix<float, 4, 4> worldMatrix;

	float movingSpeed;
};