#include "RayCast.h"
#include "MathUtils.h"
#include "Scene.h"
#include "Object.h"
#include "Logger.h"
#include "Transform.h"


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

void RayCast::Transform(float scale, Ray& ray) {

}
Ray RayCast::Transform(const RigidTransform& transform, const Ray& ray) {
	return Ray(transform.TransformPoint(ray.GetPosition()), transform.TransformVector(ray.GetDirection()), 1000.0f);
}

void RayCast::Detransform(float scale, Ray& ray) {
	ray.SetPosition(ray.GetPosition() * scale);
	ray.SetDirection(ray.GetDirection() / (Max(scale, 0.1f)));
}

Ray RayCast::Detransform(const RigidTransform& transform, const Ray& ray) {
	return Ray(transform.DetransformPoint(ray.GetPosition()), transform.DetransformVector(ray.GetDirection()), 1000.0f);
}

GameObject* RayCast::HitTest(Object& target, float x, float y, int screenWidth, int screenHeight) {
	Vec3f rayDirection = CalculateRayDirection(targetScene, x, y, screenWidth, screenHeight);
	LogInfo(L"Ray: x = %5lf, y = %5lf, z = %5lf\n", rayDirection.x, rayDirection.y, rayDirection.z);

	Matrix<float, 4, 4> inversedViewMatrix = Inverse(targetScene.GetViewMatrix());
	

	Vec3f rayWorldPos = Vec3f();
	rayWorldPos.x = inversedViewMatrix.value[12];
	rayWorldPos.y = inversedViewMatrix.value[13];
	rayWorldPos.z = inversedViewMatrix.value[14];

	Ray ray(rayWorldPos, rayDirection, 1000.0f);

	if (dynamic_cast<Scene*>(&target)) {
		for (size_t iObj = 0; iObj < targetScene.GetObjectCount(); iObj++) {
			if (targetScene.IntersectObjects(ray)) {
				return targetScene.GetGameObject(iObj);
			}
		}
	}else if (Gizmos* pGizmo = dynamic_cast<Gizmos*>(&target)) {
		double distance = 0.0;
		const RigidTransform& gizmoTransform = *pGizmo->transform;
		//float drawScale = pGizmo->ScaleByDistanceToTarget(targetScene.GetCameraPosition(), 1.0f, 0.0f);

		//Ray newRay = Detransform(gizmoTransform, ray);
		//Detransform(drawScale, newRay);

		if (pGizmo->Intersect(ray, distance)) {
			return pGizmo;
		}
	}
	
	return nullptr;
}
Vec2f RayCast::GetNormalizedDeviceCoords(float x, float y, int screenWidth, int screenHeight) {
	float newX = (2.0f * x) / screenWidth - 1.0f;
	float newY = 1.0f - (2.0f * y) / screenHeight;
	return Vec2f(newX, newY);
}

Vec3f RayCast::CalculateRayDirection(const Scene& targetScene, float x, float y, int screenWidth, int screenHeight) {
	float newX = x; float newY = y;
	Vec2f normalizedCoords = GetNormalizedDeviceCoords(x, y, screenWidth, screenHeight);
	Vec4f rayClip = Vec4f(normalizedCoords.x, normalizedCoords.y, -1.0f, 1.0f);
	Vec4f rayEye = ConvertToEyeCoords(rayClip, targetScene.GetProjectionMatrix());
	Vec3f worldRayDirection = ConvertToWorldCoords(rayEye, targetScene.GetViewMatrix());

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
