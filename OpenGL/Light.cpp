#include "Light.h"


BaseLight::BaseLight()
	: position(0.0f, 0.0f, 0.0f)
	, diffuseColor(1.0f, 1.0f, 1.0f, 0.0f)
	, ambientIntensity(1.0f)
	, diffuseIntensity(1.0f)
{

}



DirectionalLight::DirectionalLight() 
	:baseLight()
	,direction(Vec3f::FORWARD)
{

}



Attenuation::Attenuation() 
	: constant(0.0f)
	, linear(1.0f)
	, quadratic(0.0f)
{

}

PointLight::PointLight() 
	: baseLight()
	, attenuation()
{

}


SpotLight::SpotLight() 
	: baseLight()
	, attenuation()
	, spotCutoff(45.0f)
	, spotExponent(0.0f)
	, spotDirection(Vec3f::FORWARD)
{

}