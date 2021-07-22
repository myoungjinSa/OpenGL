#include "Material.h"

Material::Material()
	: diffuseAlbedo()
	, ambientAlbedo()
	, specularAlbedo()
{

}

Material::Material(const Vec3f& diffuse, const Vec4f& albedo, const Vec3f& specular, const std::pair<TextureType, unsigned int>& mapInfo) 
	: diffuseAlbedo(diffuse)
	, ambientAlbedo(albedo)
	, specularAlbedo(specular)
{
	maps.clear();
	maps.push_back(mapInfo);
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

void Material::SetTextureMap(const std::pair<TextureType, unsigned int>& textureMap) {
	auto mapIter = maps.begin();
	while (mapIter != maps.end()) {
		if ((*mapIter).first == textureMap.first)
			maps.erase(mapIter);
	}

	maps.push_back(textureMap);
}

unsigned int Material::GetTextureUnit(TextureType textureType) const{
	auto mapIter = maps.begin();
	while (mapIter != maps.end()) {
		if ((*mapIter).first == textureType)
			return (*mapIter).second;
	}
	assert(0);

	return 0;
}