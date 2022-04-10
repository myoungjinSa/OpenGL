#pragma once
#include "Matrix.h"
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

	Vec3f GetDirection() const { return direction; }
	void SetDirection(const Vec3f& dir) { direction = dir; }
	
	Vec3f GetPosition() const { return position; }
	void SetPosition(const Vec3f& pos) { position = pos; }

	bool IsValid() const { return 0.0f < length ? true : false; }
private:
	float length;
	Vec3f direction;
	Vec3f position;
};

class RayCast {
public:
	RayCast();
	~RayCast();

	void		Transform(float scale, Ray& ray);
	Ray			Transform(const RigidTransform& transform, const Ray& ray);
	
	void		Detransform(float scale, Ray& ray);
	Ray			Detransform(const RigidTransform& transform, const Ray& ray);

	Ray			GetRay(float x, float y, int screenWidth, int screenHeight, const Matrix<float, 4, 4>& viewMatrix, const Matrix<float, 4, 4>& projectionMatrix);
	//GameObject* HitTest(Object& target, float x, float y, int screenWidth, int screenHeight, const Matrix<float, 4, 4>& viewMatrix, const Matrix<float, 4, 4>& projectionMatrix);
private:
	Vec3f CalculateRayDirection(const Matrix<float, 4, 4>& viewMatrix, const Matrix<float, 4, 4>& projectionMatrix, float x, float y, int screenWidth, int screenHeight);
	Vec2f GetNormalizedDeviceCoords(float x, float y, int screenWidth, int screenHeight);
	Vec4f ConvertToEyeCoords(const Vec4f& clipCoords, const Matrix<float, 4, 4>& projectionMatrix) const;
	Vec3f ConvertToWorldCoords(const Vec4f& eyeCoords, const Matrix<float, 4, 4>& viewMatrix) const;
};