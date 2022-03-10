#pragma once
#include "Matrix.h"
class Light
{
public:
	Light();
	Light(const Light& other) = delete;
	~Light();

	void SetPosition(float x, float y, float z);
	void SetPosition(const Vec3f& pos);

	void SetDiffuseColor(const Vec4f& diffuse);
	void SetAmbientLight(const Vec4f& ambient);
	void SetSpecularLight(const Vec4f& specular);
	void SetDirection(const Vec3f& direction);
	
	Vec4f GetDiffuseLight() const;
	Vec4f GetAmbientLight() const;
	Vec4f GetSpecularLight() const;
	Vec3f GetDirection() const;
	Vec3f GetPosition() const;
private:
	Vec4f diffuseColor;
	Vec4f ambientColor;
	Vec4f specularColor;
	float specularPower;

	Vec3f position;
	Vec3f direction;
};



