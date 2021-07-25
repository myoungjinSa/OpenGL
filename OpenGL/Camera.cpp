#include "Camera.h"
#include "Input.h"
#include "Logger.h"

Camera::Camera()
	:Object()
	,width(0)
	,height(0)
	,viewMatrix()
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
	Vec3f up, lookAt;
	float yaw, pitch, roll;
	
	up.SetXYZ(0.0f, 1.0f, 0.0f);
	lookAt.SetXYZ(0.0f, 0.0f, 1.0f);
	
	const float rotationSpeed = 0.0174532925f;
	Vec3f rotation = transform.get()->GetRotation();

	pitch = rotation.x + Input::GetXAngle() * rotationSpeed;
	yaw = rotation.y + Input::GetYAngle() * rotationSpeed;
	roll = rotation.z * rotationSpeed;

	Matrix<float, 3, 3> rotationMatrix = transform.get()->GetRotationMatrix();
	MatrixRotationYawPitchRoll(rotationMatrix, yaw, pitch, roll);
	TransformCoord(lookAt, rotationMatrix);
	TransformCoord(up, rotationMatrix);
	Vec3f moveOffset;
	float movingSpeed = transform.get()->GetMovingSpeed();
	if (Input::IsKeyDown(KEY_D)) {
		moveOffset.x += movingSpeed * deltaTime;
	}
	else if (Input::IsKeyDown(KEY_A)) {
		moveOffset.x -= movingSpeed * deltaTime;
	}
	else if (Input::IsKeyDown(KEY_W)) {
		moveOffset.y += movingSpeed * deltaTime;
	}
	else if (Input::IsKeyDown(KEY_S)) {
		moveOffset.y -= movingSpeed * deltaTime;
	}
	Vec3f pos = GetPosition();
	pos += moveOffset;

	lookAt.SetXYZ(pos.x + lookAt.x, pos.y + lookAt.y, pos.z + lookAt.z);
	transform.get()->SetPosition(pos);
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
	Vec3f position = transform.get()->GetPosition();
	zAxis = Normalize(lookAt - position);

	xAxis = Cross(up, zAxis);
	xAxis = Normalize(xAxis);

	yAxis = Cross(zAxis, xAxis);

	float xPos = DotProduct(xAxis, position) * -1.0f;
	float yPos = DotProduct(yAxis, position) * -1.0f;
	float zPos = DotProduct(zAxis, position) * -1.0f;

	viewMatrix.value[0]  = xAxis.x;
	viewMatrix.value[1]  = yAxis.x;
	viewMatrix.value[2]  = zAxis.x;
	viewMatrix.value[3]  = 0.0f;
						 
	viewMatrix.value[4]  = xAxis.y;
	viewMatrix.value[5]  = yAxis.y;
	viewMatrix.value[6]  = zAxis.y;
	viewMatrix.value[7]  = 0.0f;

	viewMatrix.value[8]  = xAxis.z;
	viewMatrix.value[9]  = yAxis.z;
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

void Camera::BuildPerspectiveFovLHMatrix(Matrix<float, 4, 4>& matrix, float screenNear, float screenDepth) {
	float fov = GetFov();
	matrix[0] = GetAspectRatio() * (1.0f / (tan(fov * 0.5f)));
	matrix[1] = 0.0f;
	matrix[2] = 0.0f;
	matrix[3] = 0.0f;

	matrix[4] = 0.0f;
	matrix[5] = 1.0f / tan(fov * 0.5f);
	matrix[6] = 0.0f;
	matrix[7] = 0.0f;

	matrix[8] = 0.0f;
	matrix[9] = 0.0f;
	matrix[10] = screenDepth / (screenDepth - screenNear);
	matrix[11] = 1.0f;

	matrix[12] = 0.0f;
	matrix[13] = 0.0f;
	matrix[14] = (-screenNear * screenDepth) / (screenDepth - screenNear);
	matrix[15] = 0.0f;
}

