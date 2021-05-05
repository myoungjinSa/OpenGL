#include "Light.h"


Light::Light() {

}

Light::~Light() {

}

void Light::SetDiffuseColor(const Vec4f& _diffuse) {
	color = _diffuse;
}

void Light::SetDirection(const Vec3f& _direction) {
	direction = _direction;
}



void Light::GetDiffuseColor(float* diffuseColor) {
	diffuseColor[0] = color.x;
	diffuseColor[1] = color.y;
	diffuseColor[2] = color.z;
	diffuseColor[3] = color.w;
}

void Light::GetDirection(float* _direction) {
	_direction[0] = direction.x;
	_direction[1] = direction.y;
	_direction[2] = direction.z;
}

