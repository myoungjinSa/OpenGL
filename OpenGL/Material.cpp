#include "Material.h"

Material::Material()
	: diffuse()
	, ambient()
	, specular()
{

}

Material::Material(const Vec3f& diffuse, const Vec4f& _ambient, const Vec3f& specular)
	: diffuse(diffuse)
	, ambient(_ambient)
	, specular(specular)
{

}

Material::Material(const Vec3f& diffuse, const Vec4f& albedo, const Vec3f& specular, const std::pair<TextureType, unsigned int>& mapInfo) 
	: diffuse(diffuse)
	, ambient(albedo)
	, specular(specular)
{
	maps.clear();
	maps.push_back(mapInfo);
}

Material::~Material() {

}

void Material::SetDiffuse(const Vec3f& _diffuse) {
	diffuse = _diffuse;
}

void Material::SetAmbient(const Vec4f& _ambient) {
	ambient = _ambient;
}

void Material::SetSpecular(const Vec3f& _specular) {
	specular = _specular;
}

bool Material::DoesHaveTexture(TextureType textureType) const{
	auto mapIter = std::find_if(maps.begin(), maps.end(), [textureType](const std::pair<TextureType, unsigned int>& element) {
		if (element.first == textureType)
			return true;
		
		return false;
	});

	if (mapIter == maps.end())
		return false;

	return true;
}
void Material::SetTextureMap(const std::pair<TextureType, unsigned int>& textureMap) {
	auto mapIter = maps.begin();
	while (mapIter != maps.end()) {
		if ((*mapIter).first == textureMap.first)
			maps.erase(mapIter);

		mapIter++;
	}

	maps.push_back(textureMap);
}

unsigned int Material::GetTextureUnit(TextureType textureType) const{
	auto mapIter = maps.begin();
	while (mapIter != maps.end()) {
		if ((*mapIter).first == textureType)
			return (*mapIter).second;

		mapIter++;
	}
	assert(0);

	return 0;
}