#include "Common.h"
#include "Terrain.h"
#include "String/String.h"
#include "PictureFile.h"
#include "Shader.h"
#include "Logger.h"
#include "Renderer.h"
#include "Material.h"
#include "BoundingVolume.h"
#include "Texture.h"

TerrainHeightImage::TerrainHeightImage(const Vec3f& _scale, float _minHeight, float _maxHeight)
	: pHeightMapPixels(nullptr), width(0), length(0), scale(_scale), minHeight(_minHeight), maxHeight(_maxHeight)
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

	width = heightMap.GetWidth();
	length = heightMap.GetHeight();
	
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
	if ((nx < 0.0f) || (nz < 0.0f) || (width - 1 <= nx) || (length - 1 <= nz))
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

	int integer = int(height) % int(ceil(maxHeight));
	float remainder = height - int(height);
	height = (float)integer + remainder;
	return height;
}

Terrain::Terrain(int _gridX, int _gridZ)
	: gridX(_gridX), gridZ(_gridZ)
{}
	
bool Terrain::Initialize(Renderer& renderer) {
	GameObject::Initialize(renderer);

	AddComponent<PhongShader>();
	shader = GetComponent<Shader>();
	if (!shader)
		return false;

	if (!shader->Initialize(renderer)) {
		LogError(L"Could not initialize the Default Shader\n");
		return false;
	}

	Vec3f scale = Vec3f(8.0f, 1.0f, 8.0f);
	std::unique_ptr<TerrainHeightImage> heightImage = std::make_unique<TerrainHeightImage>(scale, -2.0f, 8.0f);
	if (!heightImage->Load(L"coast_sand_rocks.jpg")) {
		assert(0);
		return false;
	}
	
	MeshBuilder meshBuilder;
	meshes.push_back(std::make_shared<Mesh>());
	meshBuilder.Begin();

	int xTileCount = gridX;
	int zTileCount = gridZ;
	meshBuilder.AddGrid(0, 0, xTileCount, zTileCount, Vec3f(-(xTileCount * scale.x) / 2.0f, -8.0f, -(zTileCount * scale.z) / 2.0f), scale, RGBA::FOREST_GREEN, *heightImage);
	meshBuilder.CopyToMesh(renderer, *meshes.back(), &Vertex::BindVertexBuffer, &Vertex::Copy, sizeof(Vertex), Mesh::TriangleType::MeshType_Strip);
	meshBuilder.End();

	Vec3f diffuseColor(0.8f, 0.85f, 0.85f);
	Vec4f ambientColor(0.3f, 0.3f, 0.3f, 1.0f);
	Vec3f specularColor(1.0f, 1.0f, 1.0f);

	diffuseMap = TextureLoader::GetTexture(renderer, L"coast_sand_rocks.jpg");

	renderer.AllocateTextures(diffuseMap->textureID, 1);
	renderer.BindTexture(diffuseMap->GetTextureID());
	renderer.SetImage(GL_TEXTURE_2D, diffuseMap->GetPicture().GetMemory(), diffuseMap->GetPicture().GetWidth(), diffuseMap->GetPicture().GetHeight());
	renderer.SetSampleMode();
	renderer.SetFiltering();

	material = std::make_shared<Material>(diffuseColor, ambientColor, specularColor, std::make_pair(Material::TextureType::TEXTURE_DIFFUSE, diffuseMap->GetTextureID()));

	return true;
}

void Terrain::Shutdown(Renderer& renderer) {
	GameObject::Shutdown(renderer);
	if (shader)
		shader->Shutdown(renderer);

	for (auto& mesh : meshes) {
		mesh->Shutdown(renderer);
	}
}

void Terrain::Render(Renderer& renderer) {

}

void Terrain::Render(Renderer& renderer, ShaderParameter& shaderParam) {
	GameObject::Render(renderer, shaderParam);

	shader->Render(renderer, shaderParam);

	for (auto& mesh : meshes) {
		mesh->Render(renderer);
	}
	//boundingVolume->Render(renderer, shaderParam.viewMatrix, shaderParam.projectionMatrix);
}

void Terrain::Update(float deltaTime) {

}