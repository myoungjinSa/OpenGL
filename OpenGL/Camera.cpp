#include "Camera.h"

Camera::Camera() 
	:position(0.0f, 0.0f, 0.0f), rotation(0.0f, 0.0f, 0.0f)
{

}
Camera::~Camera() {

}

void Camera::SetPosition(float x, float y, float z) {
	position.SetValues(Vec3f(x, y, z));
}

Vec3f Camera::GetPosition() const {
	return position;
}

void Camera::SetRotation(float x, float y, float z) {
	rotation.SetValues(Vec3f(x, y, z));
}

Vec3f Camera::GetRotation() const {
	return rotation;
}

void Camera::Render() {
	Vec3f up, pos, lookAt;
	float yaw, pitch, roll;
	Matrix<float, 3, 3> rotationMatrix;
	
	//Setup the vector that points upwards.
	up.SetValues(Vec3f(0.0f, 1.0f, 0.0f));
	
	//Setup the position of the camera in the world
	pos = GetPosition();

	//Setup where the camera is looking by default.
	lookAt.SetValues(Vec3f(0.0f, 0.0f, 1.0f));

	//Set the yaw (Y axis), pitch(X axis) and roll(Z axis) roations in radians

	pitch = rotation.value[0] * 0.0174532925f;
	yaw = rotation.value[1] * 0.0174532925f;
	roll = rotation.value[2] * 0.0174532925f;

	//Create the rotation matrix from the yawm pitch, and roll values
	MatrixRotationYawPitchRoll(rotationMatrix, yaw, pitch, roll);

	// Transform the lookAt and up vector by the rotation matrix 
	//so the view is correctly rotated at the origin.
	TransformCoord(lookAt, rotationMatrix);
	TransformCoord(up, rotationMatrix);

	//Translate the rotated camera position to the location of the viewer.
	lookAt.SetValues(Vec3f(pos.value[0] + lookAt.value[0],
							pos.value[1] + lookAt.value[1],
							pos.value[2] + lookAt.value[2]));

	//Finally create the view matrix from the three updated vectors.
	BuildViewMatrix(pos, lookAt, up);
}

void Camera::MatrixRotationYawPitchRoll(Matrix<float, 3, 3>& matrix, float yaw, float pitch, float roll) {
	float cYaw, cPitch, cRoll, sYaw, sPitch, sRoll;

	cYaw = cosf(yaw);
	cPitch = cosf(pitch);
	cRoll = cosf(roll);

	sYaw = sinf(yaw);
	sPitch = sinf(pitch);
	sRoll = sinf(roll);

	//Calculate the yaw, pitch , roll rotation matrix.
	matrix[0] = (cRoll * cYaw) + (sRoll * sPitch * sYaw);
	matrix[1] = (sRoll * cPitch);
	matrix[2] = (cRoll * -sYaw) + (sRoll * sPitch * cYaw);

	matrix[3] = (-sRoll * cYaw) + (cRoll * sPitch * sYaw);
	matrix[4] = (cRoll * cPitch);
	matrix[5] = (sRoll * sYaw) + (cRoll * sPitch * cYaw);

	matrix[6] = (cPitch * sYaw);
	matrix[7] = -sPitch;
	matrix[8] = (cPitch * cYaw);
}

void Camera::TransformCoord(Vec3f& vec, Matrix<float, 3, 3>& matrix) {
	float x, y, z;

	//Transform the vector by the 3x3 matrix.
	x = (vec.value[0] * matrix[0]) + (vec.value[1] * matrix[3]) + (vec.value[2] * matrix[6]);
	y = (vec.value[0] * matrix[1]) + (vec.value[1] * matrix[4]) + (vec.value[2] * matrix[7]);
	z = (vec.value[0] * matrix[2]) + (vec.value[1] * matrix[5]) + (vec.value[2] * matrix[8]);

	vec.value[0] = x;
	vec.value[1] = y;
	vec.value[2] = z;
}


void Camera::BuildViewMatrix(Vec3f pos, Vec3f lookAt, Vec3f up) {


}