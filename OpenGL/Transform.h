#pragma once
#include <array>
#include "Types.h"
#include "Component.h"
#include "Quaternion.h"

class Object;
class RigidTransform : public Component 
{
public:
	RigidTransform(Object* owner);

	Vec3f TransformVector(const Vec3f& vec)const;
	Vec3f TransformPoint(const Vec3f& point)const;
	Vec3f DetransformVector(const Vec3f& vec)const;
	Vec3f DetransformPoint(const Vec3f& point)const;

	void AddPosition(const Vec3f& _position);
	void AddPosition(float x, float y, float z) { AddPosition(Vec3f(x, y, z)); }
	void SetPosition(const Vec3f& _position);
	void SetPosition(float x, float y, float z) { SetPosition(Vec3f(x, y, z)); }
	Vec3f GetPosition() const { return position; }

	void SetOrientation(const Vec4d& _orientation) { orientation = _orientation; }
	Vec4d GetOrientation() const { return orientation.GetVector(); }

	Matrix<float, 3, 3> GetRotationMatrix() const;

	//Vec4f Rotate(const Vec4f& pos, const Vec3f& pivot, float pitch, float yaw, float roll);
	void Rotate(float pitch, float yaw, float roll);
	void Rotate(const Quaternion& q);
	void Rotate(const Matrix<float, 3, 3>& rotationMatrix);
	
	void SetScale(float sx, float sy, float sz) { SetScale(Vec3f(sx, sy, sz)); }
	void SetScale(const Vec3f& size);
	Vec3f GetScale() const { return scale; }

	void Move(const Vec3f& offset);
	void Move(const Vec3f& direction, float elapsedTime);

	void SetLookRightUp(const Vec3f& _look, const Vec3f& _right, const Vec3f& _up);
	Vec3f GetLook() const { return look; }
	Vec3f GetRight() const { return right; }
	Vec3f GetUp() const { return up; }

	void  SetMovingSpeed(float speed) { movingSpeed = speed; }
	float GetMovingSpeed() const { return movingSpeed; }

	Matrix<float, 4, 4> GetWorldMatrix() const;
	void CalculateRotationMatrix(Matrix<float, 4, 4>& rotationMatrix, float pitch, float yaw, float roll, bool bUseQuaternion = true);

	void SetTransform();
private:
	Vec3f position;
	Quaternion orientation;
	Vec3f scale;

	Vec3f look;
	Vec3f up;
	Vec3f right;

	float movingSpeed;
	
	Matrix<float, 4, 4> worldMatrix;
};