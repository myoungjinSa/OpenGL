#pragma once
#include "Matrix.h"
class Light
{
public:
	Light();
	Light(const Light& other) = delete;
	~Light();

	//void SetDiffuseColor(RGBA _color);
	void SetDiffuseColor(const Vec4f& diffuse);
	void SetDirection(const Vec3f& direction);
	
	void GetDiffuseColor(float* diffuseColor);
	void GetDirection(float* direction);
private:
	Vec4f color;
	Vec3f position;
	Vec3f direction;
};

