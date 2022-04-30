#include "RayCast.h"
#include "MathUtils.h"
#include "Object.h"
#include "Logger.h"
#include "Transform.h"

Ray::Ray() 
	: length(1.0f)
	, line()
{

}
Ray::Ray(const Vec3f& _position, const Vec3f& _direction, float _length) 
	: line(_position, _direction), length(_length)
{

}

Ray::~Ray() {

}


///////////////////////////////////////////////////////////////////////////
//RayCast

RayCast::RayCast()
{

}

RayCast::~RayCast() {

}

void RayCast::Transform(float scale, Ray& ray) {

}
Ray RayCast::Transform(const RigidTransform& transform, const Ray& ray) {
	return Ray(transform.TransformPoint(ray.GetPosition()), transform.TransformVector(ray.GetDirection()), 10000.0f);
}

void RayCast::Detransform(float scale, Ray& ray) {
	ray.SetPosition(ray.GetPosition() * scale);
	ray.SetDirection(ray.GetDirection() / (Max(scale, 0.1f)));
}

Ray RayCast::Detransform(const RigidTransform& transform, const Ray& ray) {
	return Ray(transform.DetransformPoint(ray.GetPosition()), transform.DetransformVector(ray.GetDirection()), 10000.0f);
}

Ray RayCast::GetRay(float x, float y, size_t screenWidth, size_t screenHeight, const Matrix<float, 4, 4>& viewMatrix, const Matrix<float, 4, 4>& projectionMatrix) {
	Vec3f rayDirection = CalculateRayDirection(viewMatrix, projectionMatrix, x, y, screenWidth, screenHeight);
		
	Matrix<float, 4, 4> inversedViewMatrix = Inverse(viewMatrix);
	
	Vec3f rayWorldPos = Vec3f();
	rayWorldPos.x = inversedViewMatrix.value[12];
	rayWorldPos.y = inversedViewMatrix.value[13];
	rayWorldPos.z = inversedViewMatrix.value[14];
	
	Ray ray(rayWorldPos, rayDirection, 10000.0f);
	return ray;
}
Vec2f RayCast::GetNormalizedDeviceCoords(float x, float y, int screenWidth, int screenHeight) {
	float newX = (2.0f * x) / screenWidth - 1.0f;
	float newY = 1.0f - (2.0f * y) / screenHeight;
	return Vec2f(newX, newY);
}

Vec3f RayCast::CalculateRayDirection(const Matrix<float, 4, 4>& viewMatrix, const Matrix<float, 4, 4>& projectionMatrix, float x, float y, int screenWidth, int screenHeight) {
	float newX = x; float newY = y;
	Vec2f normalizedCoords = GetNormalizedDeviceCoords(x, y, screenWidth, screenHeight);
	Vec4f rayClip = Vec4f(normalizedCoords.x, normalizedCoords.y, -1.0f, 1.0f);
	Vec4f rayEye = ConvertToEyeCoords(rayClip, projectionMatrix);
	Vec3f worldRayDirection = ConvertToWorldCoords(rayEye, viewMatrix);
	
	return worldRayDirection;
}
Vec4f RayCast::ConvertToEyeCoords(const Vec4f& rayClip, const Matrix<float, 4, 4>& _projectionMatrix) const {
	Matrix<float, 4, 4> projectionMatrix = _projectionMatrix;
	Matrix<float, 4, 4> invertedProjection = Inverse(projectionMatrix);
	

	Vec4f rayEye = ::Transform(invertedProjection, rayClip);
	return Vec4f(rayEye.x, rayEye.y, 1.0f, 0.0f);
}

Vec3f RayCast::ConvertToWorldCoords(const Vec4f& rayEye, const Matrix<float, 4, 4>& _viewMatrix) const {
	Matrix<float, 4, 4> viewMatrix = _viewMatrix;
	Matrix<float, 4, 4> invertedViewMatrix = Inverse(viewMatrix);
	Vec4f rayWorld = ::Transform(invertedViewMatrix, rayEye);


	return Normalize(Vec3f(rayWorld.x, rayWorld.y, rayWorld.z));
}
