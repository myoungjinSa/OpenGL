#pragma once
#include "Matrix.h"
#include "Shader.h"

class GameObject;
class Scene;


class Ray {
public:
	Ray() = delete;
	Ray(const Vec3f& position, const Vec3f& direction, float length);
	~Ray();

	void SetLength(float _length) { length = _length; }
	float GetLength() const { return length; }

	Vec3f GetDirection() const { return direction; }
	void SetDirection(const Vec3f& dir) { direction = dir; }
	
	Vec3f GetPosition() const { return position; }
	void SetPosition(const Vec3f& pos) { position = pos; }


private:
	float length;
	Vec3f direction;
	Vec3f position;

};

class RayCast {
public:
	RayCast(Scene& _targetScene);
	~RayCast();

	GameObject* HitTest(float x, float y, int screenWidth, int screenHeight);
private:
	Vec3f CalculateRay(const Scene& targetScene, float x, float y, int screenWidth, int screenHeight);
	Vec2f GetNormalizedDeviceCoords(float x, float y, int screenWidth, int screenHeight);
	Vec4f ConvertToEyeCoords(const Vec4f& clipCoords, const Matrix<float, 4, 4>& projectionMatrix) const;
	Vec3f ConvertToWorldCoords(const Vec4f& eyeCoords, const Matrix<float, 4, 4>& viewMatrix) const;

	Scene& targetScene;
};