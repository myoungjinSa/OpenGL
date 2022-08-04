#pragma once
#include "Matrix.h"
//class Light
//{
//public:
//	Light();
//	Light(const Light& other) = delete;
//	~Light();
//
//	void SetPosition(float x, float y, float z);
//	void SetPosition(const Vec3f& pos);
//
//	void SetDiffuseColor(const Vec4f& diffuse);
//	void SetAmbientLight(const Vec4f& ambient);
//	void SetSpecularLight(const Vec4f& specular);
//	void SetDirection(const Vec3f& direction);
//	
//	Vec4f GetDiffuseLight() const;
//	Vec4f GetAmbientLight() const;
//	Vec4f GetSpecularLight() const;
//	Vec3f GetDirection() const;
//	Vec3f GetPosition() const;
//private:
//	Vec4f diffuseColor;
//	Vec4f ambientColor;
//	Vec4f specularColor;
//	float specularPower;
//
//	Vec3f position;
//	Vec3f direction;
//};
//

class BaseLight {
public:
	BaseLight();

	void SetDiffuseIntensity(float _diffuseIntensity) { diffuseIntensity = _diffuseIntensity; }
	float GetDiffuseIntensity() const { return diffuseIntensity; }

	void SetAmbientIntensity(float _ambientIntensity) { ambientIntensity = _ambientIntensity; }
	float GetAmbientIntensity() const { return ambientIntensity; }

	void SetDiffuseColor(const Vec4f _diffuseColor) { diffuseColor = _diffuseColor; }
	Vec4f GetDiffuseColor() const { return diffuseColor; }

	void SetPosition(const Vec3f& _position) { position = _position; }
	Vec3f GetPosition() const { return position; }
protected:
	Vec3f position;
	Vec4f diffuseColor;
	float ambientIntensity;
	float diffuseIntensity;
};

class DirectionalLight {
public:
	BaseLight baseLight;
	
	DirectionalLight();
	
	void SetDirection(const Vec3f& _direction) { direction = _direction; }
	Vec3f GetDirection() const { return direction; }
protected:
	Vec3f direction;
};

class Attenuation {
public:
	Attenuation();

	float constant;
	float linear;
	float quadratic;
};

class PointLight{
public:
	PointLight();
	BaseLight baseLight;
	Attenuation attenuation;
};

class SpotLight {
public:
	SpotLight();
	BaseLight baseLight;
	Attenuation attenuation;
	float spotCutoff;
	float spotExponent;
	Vec3f spotDirection;
};
