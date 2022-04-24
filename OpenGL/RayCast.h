#pragma once
#include "Types.h"
#include "Shader.h"

class GameObject;
class Scene;
class RigidTransform;
class Ray{
public:
	Ray();
	Ray(const Vec3f& position, const Vec3f& direction, float length);
	~Ray();

	void SetLength(float _length) { length = _length; }
	float GetLength() const { return length; }

	Vec3f GetDirection() const { return line.direction; }
	void SetDirection(const Vec3f& dir) { line.direction = dir; }
	
	Vec3f GetPosition() const { return line.point; }
	void SetPosition(const Vec3f& pos) { line.point = pos; }

	bool IsValid() const { return 0.0f < length ? true : false; }

	Linef GetLine() const { return line; }
private:
	float length;
	Linef line;
};

class RayCast {
public:
	RayCast();
	~RayCast();

	void		Transform(float scale, Ray& ray);
	Ray			Transform(const RigidTransform& transform, const Ray& ray);
	
	void		Detransform(float scale, Ray& ray);
	Ray			Detransform(const RigidTransform& transform, const Ray& ray);

	Ray			GetRay(float x, float y, size_t screenWidth, size_t screenHeight, const Matrix<float, 4, 4>& viewMatrix, const Matrix<float, 4, 4>& projectionMatrix);
	//GameObject* HitTest(Object& target, float x, float y, int screenWidth, int screenHeight, const Matrix<float, 4, 4>& viewMatrix, const Matrix<float, 4, 4>& projectionMatrix);
private:
	Vec3f CalculateRayDirection(const Matrix<float, 4, 4>& viewMatrix, const Matrix<float, 4, 4>& projectionMatrix, float x, float y, int screenWidth, int screenHeight);
	Vec2f GetNormalizedDeviceCoords(float x, float y, int screenWidth, int screenHeight);
	Vec4f ConvertToEyeCoords(const Vec4f& clipCoords, const Matrix<float, 4, 4>& projectionMatrix) const;
	Vec3f ConvertToWorldCoords(const Vec4f& eyeCoords, const Matrix<float, 4, 4>& viewMatrix) const;
};