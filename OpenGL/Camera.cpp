#include "Camera.h"

Camera::Camera() 
	:position(0.0f, 0.0f, 0.0f), rotation(0.0f, 0.0f, 0.0f)
{

}
Camera::~Camera() {

}

void Camera::SetPosition(float x, float y, float z) {
	position.val[0] = x;
	position.val[1] = y;
	position.val[2] = z;
}

void Camera::SetPosition(float x, float y, float z) {
	rotation.val[0] = x;
	rotation.val[1] = y;
	rotation.val[2] = z;
}


void Camera::Render() {
	Vec3f up, position, lookAt;
	float yaw, pitch, roll;
	Matrix<float, 3, 3> rotation;
	
	up.val[0] = 0.0f;
	up.val[1] = 1.0f;
	up.val[2] = 0.0f;
	
}