#include "RayCast.h"
#include "MathUtils.h"
#include "Scene.h"
#include "Object.h"
#include "Logger.h"


Ray::Ray(const Vec3f& _position, const Vec3f& _direction, float _length) 
	: position(_position), direction(_direction), length(_length)
{

}

Ray::~Ray() {

}
///////////////////////////////////////////////////////////////////////////
//RayCast

RayCast::RayCast(Scene& _targetScene)
	: targetScene(_targetScene)
{

}

RayCast::~RayCast() {

}

Object* RayCast::HitTest(float x, float y, int screenWidth, int screenHeight) {
	Vec3f worldRay = CalculateRay(targetScene, x, y, screenWidth, screenHeight);
	LogInfo(L"Ray: x = %5lf, y = %5lf, z = %5lf", worldRay.x, worldRay.y, worldRay.z);
	Matrix<float, 4, 4> inversedViewMatrix = Inverse(targetScene.GetViewMatrix());
	Vec3f rayWorldPos = Vec3f();
	rayWorldPos.x = inversedViewMatrix.value[12];
	rayWorldPos.y = inversedViewMatrix.value[13];
	rayWorldPos.z = inversedViewMatrix.value[14];

	Ray ray(rayWorldPos, worldRay, 1000.0f);
	
	for (size_t iObj = 0; iObj < targetScene.GetObjectCount(); iObj++) {
		targetScene.IntersectObjects(ray);
	}
	
	return nullptr;
}
Vec2f RayCast::GetNormalizedDeviceCoords(float x, float y, int screenWidth, int screenHeight) {
	float newX = (2.0f * x) / screenWidth - 1.0f;
	float newY = 1.0f - (2.0f * y) / screenHeight;
	//LogInfo(L"newX = %5lf, newY = %5lf\n", newX, newY);
	return Vec2f(newX, newY);
}

Vec3f RayCast::CalculateRay(const Scene& targetScene, float x, float y, int screenWidth, int screenHeight) {
	float newX = x; float newY = y;
	Vec2f normalizedCoords = GetNormalizedDeviceCoords(x, y, screenWidth, screenHeight);
	Vec4f rayClip = Vec4f(normalizedCoords.x, normalizedCoords.y, -1.0f, 1.0f);
	Vec4f rayEye = ConvertToEyeCoords(rayClip, targetScene.GetProjectionMatrix());
	Vec3f worldRay = ConvertToWorldCoords(rayEye, targetScene.GetViewMatrix());
	return worldRay;
}
Vec4f RayCast::ConvertToEyeCoords(const Vec4f& rayClip, const Matrix<float, 4, 4>& _projectionMatrix) const {
	Matrix<float, 4, 4> projectionMatrix = _projectionMatrix;
	Matrix<float, 4, 4> invertedProjection = Inverse(projectionMatrix);
	Vec4f rayEye = Transform(invertedProjection, rayClip);
	return Vec4f(rayEye.x, rayEye.y, -1.0f, 0.0f);
}

Vec3f RayCast::ConvertToWorldCoords(const Vec4f& rayEye, const Matrix<float, 4, 4>& _viewMatrix) const {
	Matrix<float, 4, 4> viewMatrix = _viewMatrix;
	Matrix<float, 4, 4> invertedViewMatrix = Inverse(viewMatrix);
	Vec4f rayWorld = Transform(invertedViewMatrix, rayEye);

	return Normalize(Vec3f(rayWorld.x, rayWorld.y, rayWorld.z));
}
