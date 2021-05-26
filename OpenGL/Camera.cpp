#include "Camera.h"
#include "Input.h"

Camera::Camera()
	:Object()
{

}
Camera::~Camera() {

}

bool Camera::Initialize(Renderer& renderer) {

	return true;
}

void Camera::Render(Renderer& renderer) {

}

void Camera::Shutdown(Renderer& renderer) {

}

void Camera::Update(float deltaTime) {
	Vec3f up, pos, lookAt;
	float yaw, pitch, roll;
	Matrix<float, 3, 3> rotationMatrix;
	
	//Setup the vector that points upwards.
	up.SetXYZ(0.0f, 1.0f, 0.0f);
	
	//Setup the position of the camera in the world
	pos = GetPosition();
	
	//Set the yaw (Y axis), pitch(X axis) and roll(Z axis) roations in radians
	lookAt.SetXYZ(0.0f, 0.0f, 1.0f);

	const float rotationSpeed = 0.0174532925f;
	pitch = rotation.x + Input::GetXAngle() * rotationSpeed;
	yaw = rotation.y + Input::GetYAngle() * rotationSpeed;
	roll = rotation.z * rotationSpeed;
	
	//Create the rotation matrix from the yawm pitch, and roll values
	MatrixRotationYawPitchRoll(rotationMatrix, yaw, pitch, roll);

	// Transform the lookAt and up vector by the rotation matrix 
	//so the view is correctly rotated at the origin.
	TransformCoord(lookAt, rotationMatrix);
	TransformCoord(up, rotationMatrix);

	//Translate the rotated camera position to the location of the viewer.
	lookAt.SetXYZ(pos.x + lookAt.x, pos.y + lookAt.y, pos.z + lookAt.z);

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
	x = (vec.x * matrix[0]) + (vec.y * matrix[3]) + (vec.z * matrix[6]);
	y = (vec.x * matrix[1]) + (vec.y * matrix[4]) + (vec.z * matrix[7]);
	z = (vec.x * matrix[2]) + (vec.y * matrix[5]) + (vec.z * matrix[8]);

	vec.x = x;
	vec.y = y;
	vec.z = z;
}


void Camera::BuildViewMatrix(Vec3f pos, Vec3f lookAt, Vec3f up) {
	Vec3f xAxis, yAxis, zAxis;
	zAxis = Normalize(lookAt - position);

	xAxis = Cross(up, zAxis);
	xAxis = Normalize(xAxis);

	yAxis = Cross(zAxis, xAxis);

	float xPos = Dot(xAxis, position) * -1.0f;
	float yPos = Dot(yAxis, position) * -1.0f;
	float zPos = Dot(zAxis, position) * -1.0f;

	viewMatrix.value[0] = xAxis.x;
	viewMatrix.value[1] = yAxis.x;
	viewMatrix.value[2] = zAxis.x;
	viewMatrix.value[3] = 0.0f;

	viewMatrix.value[4] = xAxis.y;
	viewMatrix.value[5] = yAxis.y;
	viewMatrix.value[6] = zAxis.y;
	viewMatrix.value[7] = 0.0f;

	viewMatrix.value[8] = xAxis.z;
	viewMatrix.value[9] = yAxis.z;
	viewMatrix.value[10] = zAxis.z;
	viewMatrix.value[11] = 0.0f;

	viewMatrix.value[12] = xPos;
	viewMatrix.value[13] = yPos;
	viewMatrix.value[14] = zPos;
	viewMatrix.value[15] = 1.0f;
}



void Camera::GetViewMatrix(Matrix<float, 4, 4>& Matrix) const {
	for (size_t iVal = 0; iVal < Matrix.rows * Matrix.cols; iVal++) {
		Matrix[iVal] = viewMatrix[iVal];
	}
}
