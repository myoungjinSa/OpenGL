#pragma once
#include "Matrix.h"

class Object;
class Scene;

class Ray {
public:
	Ray() = delete;
	Ray(const Vec3f& direction, float length);
	~Ray();

	void SetLength(float _length) { length = _length; }
	float GetLength() const { return length; }
private:
	float length;
	Vec3f direction;
};

class RayCast {
public:
	RayCast(Scene& _targetScene);
	~RayCast();

	Object* HitTest(float x, float y, int screenWidth, int screenHeight);
	bool	IntersectObjects(const Ray& ray, const Object& object) const;
private:
	Vec3f CalculateRay(const Scene& targetScene, float x, float y, int screenWidth, int screenHeight);
	Vec2f GetNormalizedDeviceCoords(float x, float y, int screenWidth, int screenHeight);
	Vec4f ConvertToEyeCoords(const Vec4f& clipCoords, const Matrix<float, 4, 4>& projectionMatrix) const;
	Vec3f ConvertToWorldCoords(const Vec4f& eyeCoords, const Matrix<float, 4, 4>& viewMatrix) const;

	Scene& targetScene;
};