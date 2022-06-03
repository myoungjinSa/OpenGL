#pragma once

#include "Object.h"
#include "Mesh.h"

class WString;
class Picture;
class TerrainHeightImage {
public:
	TerrainHeightImage(const Vec3f& scale, float minHeight, float maxHeight);
	~TerrainHeightImage();

	int GetWidth() const { return width; }
	int GetLength() const { return length; }
	Vec3f GetScale() const { return scale; }
	Vec3f GetHeightNormal(int x, int z) const;

	float GetHeight(float fx, float fz, bool bReverseQuad = false) const;

	unsigned char* GetHeightMapPixels() const { return pHeightMapPixels; }
	bool Load(const WString& filename);
private:
	float minHeight;
	float maxHeight;

	unsigned char* pHeightMapPixels;
	int width;
	int length;
	Vec3f scale;
};


class Terrain : public GameObject {
public:
	Terrain(int gridX, int gridZ);

	bool Initialize(Renderer& renderer)override;
	void Shutdown(Renderer& renderer)override;
	void Render(Renderer& renderer)override;
	void Render(Renderer& renderer, ShaderParameter& shaderParam) override;
	
	void Update(float deltaTime)override;
private:
	const int maxSize = 800;
	const float maxHeight = 40;

	int gridX;
	int gridZ;

	
};