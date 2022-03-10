#include "Light.h"


Light::Light()
	: diffuseColor(),
	  ambientColor(),
	  specularColor(),
	  specularPower(0),
	  position(),
	  direction()
{

}

Light::~Light() {

}

void Light::SetDiffuseColor(const Vec4f& _diffuse) {
	diffuseColor = _diffuse;
}

void Light::SetAmbientLight(const Vec4f& _ambient) {
	ambientColor = _ambient;
}

void Light::SetSpecularLight(const Vec4f& _specular) {
	specularColor = _specular;
}

void Light::SetDirection(const Vec3f& _direction) {
	direction = _direction;
}

void Light::SetPosition(float x, float y, float z) {
	position.x = x;
	position.y = y;
	position.z = z;
}

void Light::SetPosition(const Vec3f& _position) {
	position = _position;
}

Vec3f Light::GetPosition()const {
	return position;
}

Vec4f Light::GetDiffuseLight() const{
	return diffuseColor;
}

Vec4f Light::GetAmbientLight() const{
	return ambientColor;
}

Vec4f Light::GetSpecularLight() const{
	return specularColor;
}

Vec3f Light::GetDirection() const{
	return direction;
}

