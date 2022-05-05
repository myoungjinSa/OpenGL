#include "Common.h"
#include "Terrain.h"
#include "String/String.h"
#include "PictureFile.h"
#include "Shader.h"
#include "Logger.h"

TerrainHeightImage::TerrainHeightImage(int _width, int _length, const Vec3f& _scale)
	: pHeightMapPixels(nullptr), width(_width), length(_length), scale(_scale)
{
	
}

TerrainHeightImage::~TerrainHeightImage() {
	if (pHeightMapPixels)
		delete[] pHeightMapPixels;
}
bool TerrainHeightImage::Load(const WString& filename) {
	if (!PictureFile::IsImageFile(filename)) {
		return false;
	}

	Picture heightMap;
	if (!PictureFile::CreatePicture(heightMap, filename)) {
		return false;
	}

	pHeightMapPixels = new unsigned char[width * length];
	
	unsigned char* pMemory = (unsigned char*)heightMap.GetMemory();
	for (int y = 0; y < length; y++) {
		for (int x = 0; x < width; x++) {
			pHeightMapPixels[x + (length - 1 - y) * width] = pMemory[x + (y * width)];
		}
	}
	return true;
}
Vec3f TerrainHeightImage::GetHeightNormal(int nx, int nz) const {
	if ((nx < 0.0f) || (nz < 0.0f) || (width <= nx) || (length <= nz))
		return Vec3f(0.0f, 1.0f, 0.0f);


	int nHeightMapIndex = nx + (nz * width);
	int xHeightMapAdd = ((nx < width - 1)) ? 1 : -1;
	int zHeightMapAdd = ((nz < length - 1)) ? width : -width;

	float height_1 = (float)pHeightMapPixels[nHeightMapIndex] * scale.y;
	float height_2 = (float)pHeightMapPixels[nHeightMapIndex + xHeightMapAdd] * scale.y;
	float height_3 = (float)pHeightMapPixels[nHeightMapIndex + zHeightMapAdd] * scale.y;

	Vec3f edge1 = Vec3f(0.0f, height_3 - height_1, scale.z);
	Vec3f edge2 = Vec3f(scale.x, height_2 - height_1, 0.0f);
	Vec3f normal = Normalize(Cross(edge1, edge2));
	return normal; 
}
float TerrainHeightImage::GetHeight(float fx, float fz, bool bReverseQuad) const {
	fx = fx / Max(0.001f, scale.x);
	fz = fz / Max(0.001f, scale.z);

	if ((fx < 0.0f) || (fz < 0.0f) || (width <= fx) || (length <= fz))
		return 0.0f;

	int x = (int)fx;
	int z = (int)fz;
	
	float bottomLeft = (float)pHeightMapPixels[x + (z * width)];
	float bottomRight = (float)pHeightMapPixels[(x + 1) + (z * width)];
	float topLeft = (float)pHeightMapPixels[x + ((z + 1) * width)];
	float topRight = (float)pHeightMapPixels[(x + 1) + ((z + 1) * width)];

	float tx = fx - x;
	float tz = fz - z;

	if (bReverseQuad) {
		if (tx <= tz) {
			bottomRight = bottomLeft + (topRight - topLeft);
		}else {
			topLeft = topRight + (bottomLeft - bottomRight);
		}
	}else {
		if (tz < (1.0f - tx)) {// z쪽에 더 가까울경우
			topRight = topLeft + (bottomRight - bottomLeft);
		}else {
			//x쪽에 더 가까울때
			bottomLeft = topLeft + (bottomRight - topRight);
		}
	}
	
	float topHeight = MathUtils::Lerp(topLeft, topRight, tx);
	float bottomHeight = MathUtils::Lerp(bottomLeft, bottomRight, tx);
	float height = MathUtils::Lerp(bottomHeight, topHeight, tz);
	return height;
}

Terrain::Terrain(int gridX, int gridZ) {

}

bool Terrain::Initialize(Renderer& renderer) {
	GameObject::Initialize(renderer);

	shader = std::make_shared<ColorShader>(this);
	if (!shader)
		return false;

	if (!shader->Initialize(renderer)) {
		LogError(L"Could not initialize the Default Shader\n");
		return false;
	}

	MeshBuilder meshBuilder;
	meshes.push_back(std::make_shared<Mesh>());
	meshBuilder.Begin();
	//meshBuilder.AddGrid();
	meshBuilder.End();



}

void Terrain::Shutdown(Renderer& renderer) {

}

void Terrain::Render(Renderer& renderer) {

}

void Terrain::Render(Renderer& renderer, ShaderParameter& shaderParam) {

}

void Terrain::Update(float deltaTime) {

}