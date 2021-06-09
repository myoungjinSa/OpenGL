#include "Material.h"

Material::Material()
	: diffuseAlbedo()
	, ambientAlbedo()
	, specularAlbedo()
	, textureUnit(0)
{

}

Material::Material(const Vec3f& diffuse, const Vec4f& albedo, const Vec3f& specular, int _textureUnit) 
	: diffuseAlbedo(diffuse)
	, ambientAlbedo(albedo)
	, specularAlbedo(specular)
	, textureUnit(_textureUnit)
{

}

Material::~Material() {

}

void Material::SetDiffuseAlbedo(const Vec3f& diffuse) {
	diffuseAlbedo = diffuse;
}

void Material::SetAmbientAlbedo(const Vec4f& ambient) {
	ambientAlbedo = ambient;
}

void Material::SetSpecularAlbedo(const Vec3f& specular) {
	specularAlbedo = specular;
}

void Material::SetTextureUnit(unsigned int _textureUnit) {
	textureUnit = _textureUnit;
}