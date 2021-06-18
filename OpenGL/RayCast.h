#pragma once
#include "Matrix.h"
class RayCast {
public:
	class Ray {
	public:
		Ray() = delete;
		Ray(const Vec3f& direction, float length);
		~Ray();

		void SetLength(float _length) { length = _length; }
		float GetLength() const { return length; }
	protected:
		float length;
		Vec3f direction;
	};

	RayCast();
	~RayCast();

private:

};