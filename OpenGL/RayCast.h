#pragma once
#include "Matrix.h"

class Object;
class Scene;
class RayCast {
public:
	class Ray {
	public:
		Ray() = delete;
		Ray(const Vec3f& direction, float length);
		~Ray();

		Vec3f CalculateRay(float x, float y, int screenWidth, int screenHeight);
		void SetLength(float _length) { length = _length; }
		float GetLength() const { return length; }
	private:
		Vec2f GetNormalizedDeviceCoords(float x, float y, int screenWidth, int screenHeight);
		Vec4f ConvertToEyeCoords(const Vec4f& clipCoords, const Matrix<float, 4, 4>& projectionMatrix);

		float length;
		Vec3f direction;
	};

	RayCast(Scene& _targetScene);
	~RayCast();

	Object* HitTest(float x, float y);
private:

	Scene& targetScene;
};