#pragma once
#include "Matrix.h"
#include <vector>

class Material {
public:
	enum class TextureType : int{
		TEXTURE_DIFFUSE,
		TEXTURE_NORMAL,
		TEXTURE_ENVIORNMENT,

		TEXTURE_COUNT
	};

	Material();
	Material(const Vec3f& diffuse, const Vec4f& albedo, const Vec3f& specular);
	Material(const Vec3f& diffuse, const Vec4f& albedo, const Vec3f& specular, const std::pair<TextureType, unsigned int>& mapInfo);
	virtual ~Material();

	Vec3f GetDiffuse() { return diffuse; }
	Vec4f GetAmbient() { return ambient; }
	Vec3f GetSpecular() { return specular; }

	void SetDiffuse(const Vec3f& diffuse);
	void SetAmbient(const Vec4f& ambient);
	void SetSpecular(const Vec3f& specular);

	bool DoesHaveTexture(TextureType textureType) const;
	void SetTextureMap(const std::pair<TextureType, unsigned int>& maps);
	unsigned int GetTextureUnit(TextureType textureType) const;
private:
	Vec3f diffuse;
	Vec4f ambient;
	Vec3f specular;
	std::vector<std::pair<TextureType, unsigned int>> maps;
};