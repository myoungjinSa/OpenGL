#pragma once
#include "Matrix.h"
#include <vector>

class Material {
public:
	enum class TextureType : int{
		TEXTURE_ALBEDO,
		TEXTURE_NORMAL,
		TEXTURE_COUNT
	};

	Material();
	Material(const Vec3f& diffuse, const Vec4f& albedo, const Vec3f& specular, const std::pair<TextureType, unsigned int>& mapInfo);
	virtual ~Material();

	Vec3f GetDiffuseAlbedo() { return diffuseAlbedo; }
	Vec4f GetAmbientAlbedo() { return ambientAlbedo; }
	Vec3f GetSpecularAlbedo() { return specularAlbedo; }

	void SetDiffuseAlbedo(const Vec3f& diffuse);
	void SetAmbientAlbedo(const Vec4f& ambient);
	void SetSpecularAlbedo(const Vec3f& specular);

	void SetTextureMap(const std::pair<TextureType, unsigned int>& maps);
	unsigned int GetTextureUnit(TextureType textureType) const;
private:
	Vec3f diffuseAlbedo;
	Vec4f ambientAlbedo;
	Vec3f specularAlbedo;
	std::vector<std::pair<TextureType, unsigned int>> maps;
};