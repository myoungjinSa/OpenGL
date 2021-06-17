#include "Texture.h"
#include "Renderer.h"
#include "Types.h"
#include <fstream>
#include <cassert>
#include "TGAReader/tga_reader.h"

Texture::Texture() 
	: loaded(false), textureID(0)
{

}

Texture::~Texture() {

}

bool Texture::Initialize(Renderer& renderer, const String& fileName, unsigned int textureUnit, bool wrap) {
	bool result;

	int texID = 0;
	//Load the targa file.
	result = LoadBMP(renderer, fileName, textureUnit, wrap, texID);
	if (!result) {
		return false;
	}
	
	textureID = (unsigned int)texID;

	return true;
}

void Texture::Shutdown() {
	if (loaded) {
		glDeleteTextures(1, &textureID);
		loaded = false;
	}
	return;
}

bool Texture::LoadBMP(Renderer& renderer, const String& fileName, unsigned int textureUnit, bool wrap, int& genTextureID) {
	std::ifstream fin(fileName.c_str(), std::ios::in | std::ios::binary);
	if (fin.fail())
		return false;

	const int headerSize = 54;
	unsigned char header[headerSize];
	fin.read(reinterpret_cast<char*>(header), headerSize);

	if (header[0] != 'B' || header[1] != 'M') {
		return false;
	}

	unsigned int dataPos = *(int*)&(header[0x0A]);
	unsigned int imageSize = *(int*)&(header[0x22]);
	unsigned int width = *(int*)&(header[0x12]);
	unsigned int height = *(int*)&(header[0x16]);

	if (imageSize == 0)
		imageSize = width * height * 3;

	if (dataPos == 0)
		dataPos = *(header + headerSize);
		

	unsigned char* pPixelData = new unsigned char[imageSize];
	fin.read(reinterpret_cast<char*>(pPixelData), imageSize);
	fin.close();

	renderer.AllocateTextures(textureID, 1);
	renderer.BindTexture(width, height, textureID, pPixelData);
	renderer.SetSampleMode(true);
	renderer.SetFiltering();

	genTextureID = textureID;
	delete[] pPixelData;
	pPixelData = nullptr;

	loaded = true;
	return loaded;
}

/////////////////////////////////////////////////////////////////////////////
std::vector<std::pair<String&&, std::shared_ptr<Texture>>> TextureLoader::textures;
TextureLoader::TextureLoader() {
	
}

TextureLoader::~TextureLoader() {
	
}

void TextureLoader::Release() {
	for (auto& tex : textures)
		tex.second->Shutdown();
}
const std::shared_ptr<Texture>& TextureLoader::GetTexture(Renderer& renderer, const String& filename) {
	bool bFind = false;
	for (size_t iTexture = 0; iTexture < textures.size(); iTexture++) {
		if (textures[iTexture].first.Compare(filename)) {
			bFind = true;
			return textures[iTexture].second;
		}
	}
	if (!bFind) {
		if (!Load(renderer, filename)) {
			assert(false);
		}
		bFind = true;
	}
	size_t index = textures.size();
	return textures[index - 1].second;
}

bool TextureLoader::Load(Renderer& renderer, const String& filename) {
	std::shared_ptr<Texture> texture = std::make_shared<Texture>();
	if (!texture)
		return false;

	if(!texture->Initialize(renderer, filename, 0,  true))
		return false;

	textures.emplace_back(std::make_pair(filename, texture));
	return true;
}

////////////////////////////////////////////////////////////////////////
RenderTarget::RenderTarget() 
	: iFrameBuffer(0)
	, size()
{

}

RenderTarget::~RenderTarget() {

}


bool RenderTarget::Create(Renderer& renderer, size_t fboCount) {
	if (fboCount == 0)
		return false;

	
	return true;
}


void RenderTarget::SetSize(const Size2u& _size) {
	size = _size;
}

void RenderTarget::SetSize(size_t width, size_t height) {
	size.width = width;
	size.height = height;
}

Size2u RenderTarget::GetSize() const {
	return size;
}