#include "Transform.h"
#include "Object.h"
#include "Quaternion.h"

RigidTransform::RigidTransform(Object* _owner)
	: Component(_owner), position(0.0f, 0.0f, 0.0f), orientation(0.0, 0.0, 0.0, 1.0), scale(1.0f, 1.0f, 1.0f), movingSpeed(1.0f)
{
	worldMatrix = Matrix<float, 4, 4>::Identity();
}

Vec3f RigidTransform::TransformVector(const Vec3f& vec)const{
	return Quaternion::Rotate(orientation, vec * scale);
}

Vec3f RigidTransform::TransformPoint(const Vec3f& point) const{
	return position + TransformVector(point);
}


Vec3f RigidTransform::DetransformPoint(const Vec3f& point) const{
	return DetransformVector(point - position);
}

Vec3f RigidTransform::DetransformVector(const Vec3f& vec) const{
	Quaternion q(vec, 1.0f);

	return Quaternion::Rotate(q.Inverse(orientation).GetVector(), vec) / Vec3d(scale.x, scale.y, scale.z);
}


void RigidTransform::AddPosition(const Vec3f& _position) {
	position += _position;
	SetTransform();
}

void RigidTransform::AddPosition(float x, float y, float z) {
	position.x += x;
	position.y += y;
	position.z += z;
	SetTransform();
}
void RigidTransform::SetPosition(const Vec3f& _position) {
	position = _position;
	SetTransform();
}

void RigidTransform::SetPosition(float x, float y, float z) {
	position = Vec3f(x, y, z);
	SetTransform();
}

Vec3f RigidTransform::GetPosition() const {
	return position;
}

void RigidTransform::GetPosition(std::array<float, 4>& _position) {
	_position[0] = position.x;
	_position[1] = position.y;
	_position[2] = position.z;
	_position[3] = 1.0f;
}

Matrix<float, 3, 3> RigidTransform::GetRotationMatrix() const {
	Matrix<float, 3, 3> rotationMatrix;
	rotationMatrix.value[0] = worldMatrix.value[0];
	rotationMatrix.value[1] = worldMatrix.value[1];
	rotationMatrix.value[2] = worldMatrix.value[2];

	rotationMatrix.value[3] = worldMatrix.value[4];
	rotationMatrix.value[4] = worldMatrix.value[5];
	rotationMatrix.value[5] = worldMatrix.value[6];

	rotationMatrix.value[6] = worldMatrix.value[8];
	rotationMatrix.value[7] = worldMatrix.value[9];
	rotationMatrix.value[8] = worldMatrix.value[10];

	return rotationMatrix;
}

void RigidTransform::Rotate(float pitch, float yaw, float roll) {
	Matrix<float, 4, 4> rotationMatrix = Matrix<float, 4, 4>::Identity();
	CalculateRotationMatrix(rotationMatrix, pitch, yaw, roll);
	worldMatrix.Multiply(rotationMatrix);
}

Vec4f RigidTransform::Rotate(const Vec3f& pos, const Vec3f& pivot, float pitch, float yaw, float roll) {
	Matrix<float, 4, 4> rotationMatrix = Matrix<float, 4 ,4>::Identity();
	CalculateRotationMatrix(rotationMatrix, pitch, yaw, roll);
	

	Vec4f rotatedVector(pos, 1.0f);
	rotatedVector = Transform(rotationMatrix, rotatedVector);
	return rotatedVector;
}

void RigidTransform::CalculateRotationMatrix(Matrix<float, 4, 4>& rotationMatrix, float pitch, float yaw, float roll, bool bUseQuaternion) const {
	if (bUseQuaternion) {
		Quaternion q;
		q.GetEuler(Vec3d(pitch, yaw, roll));
		q.Normalize();
		
		Matrix<double, 3, 3> m = q.GetRotationMatrix();
		
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

		//rotationMatrix[0] = m[0];
		//rotationMatrix[1] = m[3];
		//rotationMatrix[2] = m[6];
		//rotationMatrix[3] = 0.0f;
		//
		//rotationMatrix[4] = m[1];
		//rotationMatrix[5] = m[4];
		//rotationMatrix[6] = m[7];
		//rotationMatrix[7] = 0.0f;
		//
		//rotationMatrix[8] = m[2];
		//rotationMatrix[9] = m[5];
		//rotationMatrix[10] = m[8];
		//rotationMatrix[11] = 0.0f;
		
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

void RigidTransform::SetScale(float sx, float sy, float sz) {
	SetScale(Vec3f(sx, sy, sz));
}

void RigidTransform::SetScale(const Vec3f& _scale) {
	scale = _scale;
}

Vec3f RigidTransform::GetScale() const {
	return scale;
}

void RigidTransform::Move(const Vec3f& direction, float elapsedTime) {
	position = GetPosition() + Normalize(direction) * elapsedTime * movingSpeed;
	SetTransform();
}

void RigidTransform::SetLook(const Vec3f& look) {
	worldMatrix.value[8] = look.x;
	worldMatrix.value[9] = look.y;
	worldMatrix.value[10] = look.z;
}

void RigidTransform::SetLook(Vec3f&& look) noexcept {
	worldMatrix.value[8] = look.x;
	worldMatrix.value[9] = look.y;
	worldMatrix.value[10] = look.z;
}

void RigidTransform::SetUp(const Vec3f& up) {
	worldMatrix.value[4] = up.x;
	worldMatrix.value[5] = up.y;
	worldMatrix.value[6] = up.z;
}


void RigidTransform::SetUp(Vec3f&& up) noexcept {
	worldMatrix.value[4] = up.x;
	worldMatrix.value[5] = up.y;
	worldMatrix.value[6] = up.z;
}

void RigidTransform::SetRight(const Vec3f& right) {
	worldMatrix.value[0] = right.x;
	worldMatrix.value[1] = right.y;
	worldMatrix.value[2] = right.z;
}

void RigidTransform::SetRight(Vec3f&& right) noexcept {
	worldMatrix.value[0] = right.x;
	worldMatrix.value[1] = right.y;
	worldMatrix.value[2] = right.z;
}



Vec3f RigidTransform::GetLook() const {
	Vec3f lookVector(worldMatrix.value[8], worldMatrix.value[9], worldMatrix.value[10]);
	return Normalize(lookVector);
}

Vec3f RigidTransform::GetRight() const {
	Vec3f rightVector(worldMatrix.value[0], worldMatrix.value[1], worldMatrix.value[2]);
	return Normalize(rightVector);
}

Vec3f RigidTransform::GetUp() const {
	Vec3f upVector(worldMatrix.value[4], worldMatrix.value[5], worldMatrix.value[6]);
	return Normalize(upVector);
}

void RigidTransform::SetMovingSpeed(float speed) {
	movingSpeed = speed;
}

float RigidTransform::GetMovingSpeed() const {
	return movingSpeed;
}

const Matrix<float, 4, 4>& RigidTransform::GetWorldMatrix() const {
	return worldMatrix;
}

void RigidTransform::SetWorldMatrix(const Matrix<float, 4, 4>& _worldMatrix) {
	worldMatrix = _worldMatrix;

	SetPosition(worldMatrix[12], worldMatrix[13], worldMatrix[14]);
}

void RigidTransform::SetTransform() {
	worldMatrix[12] = position.x;
	worldMatrix[13] = position.y;
	worldMatrix[14] = position.z;
	worldMatrix[15] = 1.0f;
}