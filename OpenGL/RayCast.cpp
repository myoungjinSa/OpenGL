#include "RayCast.h"
#include "MathUtils.h"
#include "Scene.h"
#include "Object.h"


RayCast::Ray::Ray(const Vec3f& _direction, float length) 
	: direction(_direction),length(0.0f)
{

}

RayCast::Ray::~Ray() {

}
Vec3f RayCast::Ray::CalculateRay(float x, float y, int screenWidth, int screenHeight) {
	float newX = x;
	float newY = y;
	Vec2f normalizedCoords = GetNormalizedDeviceCoords(x, y, screenWidth, screenHeight);
	Vec4f clipCoords = Vec4f(normalizedCoords.x, normalizedCoords.y, -1.0f, 1.0f);
	
}

Vec2f RayCast::Ray::GetNormalizedDeviceCoords(float x, float y, int screenWidth, int screenHeight) {
	float newX = (2.0f * x) / MAX(screenWidth - 1, 1);
	float newY = (2.0f * y) / MAX(screenHeight - 1, 1);
	return Vec2f(newX, -newY);
}

Vec4f RayCast::Ray::ConvertToEyeCoords(const Vec4f& clipCoords, const Matrix<float, 4, 4>& _projectionMatrix) {
	Matrix<float, 4, 4> projectionMatrix = _projectionMatrix;
	//Matrix<float, 4, 4> invertedProjection = 

}

RayCast::RayCast(Scene& _targetScene)
	: targetScene(_targetScene)
{

}

RayCast::~RayCast() {

}

Object* RayCast::HitTest(float x, float y) {
	Matrix<float, 4, 4> viewMatrix = targetScene.GetViewMatrix();
	Matrix<float, 4, 4> projectionMatrix = targetScene.GetProjectionMatrix();

	return nullptr;
}

