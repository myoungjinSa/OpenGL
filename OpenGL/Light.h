#pragma once
#include "Matrix.h"
class Light
{
public:
	Light();
	Light(const Light& other) = delete;
	~Light();

	void SetDiffuseColor(const Vec4f& diffuse);
	void SetAmbientLight(const Vec4f& ambient);
	void SetDirection(const Vec3f& direction);
	
	void GetDiffuseColor(float* diffuseColor);
	void GetDirection(float* direction);
	void GetAmbientLight(float* ambientLight);
private:
	Vec4f diffuse;
	Vec4f ambient;

	Vec3f position;
	Vec3f direction;
};

