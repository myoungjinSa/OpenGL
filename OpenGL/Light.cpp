#include "Light.h"


Light::Light() {

}

Light::~Light() {

}

void Light::SetDiffuseColor(const Vec4f& _diffuse) {
	diffuse = _diffuse;
}

void Light::SetAmbientLight(const Vec4f& _ambient) {
	ambient = _ambient;
}

void Light::SetDirection(const Vec3f& _direction) {
	direction = _direction;
}



void Light::GetDiffuseColor(float* diffuseColor) {
	diffuseColor[0] = diffuse.x;
	diffuseColor[1] = diffuse.y;
	diffuseColor[2] = diffuse.z;
	diffuseColor[3] = diffuse.w;
}

void Light::GetAmbientLight(float* _ambientLight) {
	_ambientLight[0] = ambient.x;
	_ambientLight[1] = ambient.y;
	_ambientLight[2] = ambient.z;
	_ambientLight[3] = ambient.w;
}

void Light::GetDirection(float* _direction) {
	_direction[0] = direction.x;
	_direction[1] = direction.y;
	_direction[2] = direction.z;
}

