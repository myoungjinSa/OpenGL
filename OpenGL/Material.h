#pragma once
#include "Matrix.h"

class Material {
public:
	Material();
	Material(const Vec3f& diffuse, const Vec4f& albedo, const Vec3f& specular, int textureUnit);
	virtual ~Material();

	Vec3f GetDiffuseAlbedo() { return diffuseAlbedo; }
	Vec4f GetAmbientAlbedo() { return ambientAlbedo; }
	Vec3f GetSpecularAlbedo() { return specularAlbedo; }

	void SetDiffuseAlbedo(const Vec3f& diffuse);
	void SetAmbientAlbedo(const Vec4f& ambient);
	void SetSpecularAlbedo(const Vec3f& specular);

	void SetTextureUnit(unsigned int texture);
	unsigned int GetTextureUnit() { return textureUnit; }
private:
	Vec3f diffuseAlbedo;
	Vec4f ambientAlbedo;
	Vec3f specularAlbedo;
	unsigned int textureUnit;
};