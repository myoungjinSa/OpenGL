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

void Light::GetPosition(float* _position) {
	_position[0] = position.x;
	_position[1] = position.y;
	_position[2] = position.z;
}


void Light::GetDiffuseLight(float* _diffuseColor) {
	_diffuseColor[0] = diffuseColor.x;
	_diffuseColor[1] = diffuseColor.y;
	_diffuseColor[2] = diffuseColor.z;
	_diffuseColor[3] = diffuseColor.w;
}

void Light::GetAmbientLight(float* _ambientLight) {
	_ambientLight[0] = ambientColor.x;
	_ambientLight[1] = ambientColor.y;
	_ambientLight[2] = ambientColor.z;
	_ambientLight[3] = ambientColor.w;
}

void Light::GetSpecularLight(float* _specularLight) {
	_specularLight[0] = specularColor.x;
	_specularLight[1] = specularColor.y;
	_specularLight[2] = specularColor.z;
	_specularLight[3] = specularColor.w;
}

void Light::GetDirection(float* _direction) {
	_direction[0] = direction.x;
	_direction[1] = direction.y;
	_direction[2] = direction.z;
}

