#include "Transform.h"
#include "Object.h"
#include "Quaternion.h"


RigidTransform::RigidTransform(Object* _owner)
	: Component(_owner),
	position(0.0f, 0.0f, 0.0f),
	orientation(0.0, 0.0, 0.0, 1.0),
	scale(1.0f, 1.0f, 1.0f),
	look(Vec3f::FORWARD),
	up(Vec3f::UP),
	right(Vec3f::RIGHT),
	movingSpeed(2.0f)
{
}

Vec3f RigidTransform::TransformVector(const Vec3f& vec)const{
	return Quaternion::Rotate(orientation.GetVector(), vec * scale);
}

Vec3f RigidTransform::TransformPoint(const Vec3f& point) const{
	return position + TransformVector(point);
}


Vec3f RigidTransform::DetransformPoint(const Vec3f& point) const{
	return DetransformVector(point - position);
}

Vec3f RigidTransform::DetransformVector(const Vec3f& vec) const{
	Quaternion q(vec, 1.0f);

	return Quaternion::Rotate(q.Inverse(orientation.GetVector()).GetVector(), vec) / Vec3d(scale.x, scale.y, scale.z);
}


Matrix<float, 3, 3> RigidTransform::GetRotationMatrix() const {
	Matrix<float, 3, 3> rotationMatrix;
	rotationMatrix = orientation.GetRotationMatrix();
	return rotationMatrix;
}

void RigidTransform::Rotate(float pitch, float yaw, float roll) {
	/*Matrix<float, 4, 4> rotationMatrix = Matrix<float, 4, 4>::Identity();
	CalculateRotationMatrix(rotationMatrix, pitch, yaw, roll);*/

	orientation.GetEuler(Vec3d(pitch, yaw, roll));

	look = orientation.GetRotatedVector(look);
	up = orientation.GetRotatedVector(up);
	right = orientation.GetRotatedVector(right);
}

void RigidTransform::Rotate(const Quaternion& q) {
	orientation = q;

	Vec3f euler = q.GetEuler();
	Rotate(euler.x, euler.y, euler.z);
}

void RigidTransform::CalculateRotationMatrix(Matrix<float, 4, 4>& rotationMatrix, float pitch, float yaw, float roll, bool bUseQuaternion) {
	orientation.GetEuler(Vec3d(pitch, yaw, roll));
	orientation.Normalize();

	if (bUseQuaternion) {
		Matrix<double, 3, 3> m = orientation.GetRotationMatrix();
		rotationMatrix[0] = m[0];
		rotationMatrix[1] = m[1];
		rotationMatrix[2] = m[2];
		rotationMatrix[3] = 0.0f;

		rotationMatrix[4] = m[3];
		rotationMatrix[5] = m[4];
		rotationMatrix[6] = m[5];
		rotationMatrix[7] = 0.0f;

		rotationMatrix[8] = m[6];
		rotationMatrix[9] = m[7];
		rotationMatrix[10] = m[8];
		rotationMatrix[11] = 0.0f;
		
		rotationMatrix[12] = 0.0f;
		rotationMatrix[13] = 0.0f;
		rotationMatrix[14] = 0.0f;
		rotationMatrix[15] = 1.0f;
	}
	else {
		float cYaw, cPitch, cRoll, sYaw, sPitch, sRoll;

		cYaw = cosf(yaw);
		cPitch = cosf(pitch);
		cRoll = cosf(roll);

		sYaw = sinf(yaw);
		sPitch = sinf(pitch);
		sRoll = sinf(roll);

		//Calculate the yaw, pitch , roll rotation matrix.
		rotationMatrix[0] = (cRoll * cYaw) + (sRoll * sPitch * sYaw);
		rotationMatrix[1] = (sRoll * cPitch);
		rotationMatrix[2] = (cRoll * -sYaw) + (sRoll * sPitch * cYaw);
		rotationMatrix[3] = 0.0f;

		rotationMatrix[4] = (-sRoll * cYaw) + (cRoll * sPitch * sYaw);
		rotationMatrix[5] = (cRoll * cPitch);
		rotationMatrix[6] = (sRoll * sYaw) + (cRoll * sPitch * cYaw);
		rotationMatrix[7] = 0.0f;

		rotationMatrix[8] = (cPitch * sYaw);
		rotationMatrix[9] = -sPitch;
		rotationMatrix[10] = (cPitch * cYaw);
		rotationMatrix[11] = 0.0f;

		rotationMatrix[12] = 0.0f;
		rotationMatrix[13] = 0.0f;
		rotationMatrix[14] = 0.0f;
		rotationMatrix[15] = 1.0f;
	}
}

void RigidTransform::Move(const Vec3f& offset) {
	position = GetPosition() + offset;
}

void RigidTransform::Move(const Vec3f& direction, float elapsedTime) {
	position = GetPosition() + Normalize(direction) * elapsedTime * movingSpeed;
}

Matrix<float, 4, 4> RigidTransform::GetWorldMatrix() const {
	Matrix<float, 4, 4> worldMatrix = Matrix<float, 4, 4>::Identity(); 
	worldMatrix.value[0] = scale.x * right.x;
	worldMatrix.value[1] = right.y;
	worldMatrix.value[2] = right.z;
	
	worldMatrix.value[4] = up.x;
	worldMatrix.value[5] = scale.y * up.y;
	worldMatrix.value[6] = up.z;

	worldMatrix.value[8] = look.x;
	worldMatrix.value[9] = look.y;
	worldMatrix.value[10] = scale.z * look.z;

	worldMatrix.value[12] = position.x;
	worldMatrix.value[13] = position.y;
	worldMatrix.value[14] = position.z;
	return worldMatrix;
}
