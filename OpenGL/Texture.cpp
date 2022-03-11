#include "Texture.h"
#include "Renderer.h"
#include "Types.h"
#include <fstream>
#include <cassert>
#include "TGAReader/tga_reader.h"
#include "Picture.h"
#include "PictureFile.h"

Texture::Texture() 
	: textureID(0)
	, type(eType::TYPE_COUNT)
	, filename()
{

}

Texture::~Texture() {

}

bool Texture::Initialize(Renderer& renderer, const WString& _fileName, unsigned int textureUnit, bool wrap) {
	bool result;

	renderContext = std::make_unique<RenderContext>(renderer);
	int texID = 0;
	result = Load(renderer, _fileName, textureUnit, wrap, texID);
	if (!result) {
		return false;
	}
	filename = _fileName;
	textureID = (unsigned int)texID;

	return true;
}

bool Texture::Update(float deltaTime) {
	if (type != eType::TYPE_VIDEO)
		return true;

	renderContext->totalDeltaTime += deltaTime;
	/*if (MathUtils::IsSame(renderContext->prevFrameNo + deltaTime, renderContext->totalDeltaTime)) {
		return false;
	}*/

	renderContext->prevFrameNo = renderContext->totalDeltaTime;

	Picture picture;
	if (renderContext->preVideoFile) {
		int64_t pts = renderContext->preVideoFile->CalcPts(renderContext->totalDeltaTime);
		PictureFile::UpdateVideoFrame(picture, renderContext->preVideoFile, pts);
	}

	renderContext->renderer.BindTexture(picture.GetWidth(), picture.GetHeight(), textureID, (unsigned char*)picture.GetMemory());
	renderContext->renderer.SetSampleMode(true);
	renderContext->renderer.SetFiltering();
	
	return true;
}
void Texture::Shutdown() {
	glDeleteTextures(1, &textureID);
	return;
}

bool Texture::Load(Renderer& renderer, const WString& filename, unsigned int textureUnit, bool wrap, int& genTextureID) {
	if (!FileUtils::DoesFileExist(filename)) {
		return false;
	}
	Picture picture;
	if (PictureFile::IsImageFile(filename)) {
		if (!PictureFile::CreatePicture(picture, filename)) {
			return false;
		}
		type = eType::TYPE_IMAGE;
	}else if(PictureFile::IsVideoFile(filename)){
		int frameNo = 0;
		if (!PictureFile::CreateVideoFrame(picture, renderContext->preVideoFile, filename, frameNo)) {
			return false;
		}
		assert(renderContext->preVideoFile->IsOpened());
		renderContext->prevFrameNo = frameNo;

		type = eType::TYPE_VIDEO;
	}
	else {
		assert(0);
		return false;
	}
	
	renderer.AllocateTextures(textureID, 1);
	renderer.BindTexture(picture.GetWidth(), picture.GetHeight(), textureID, (unsigned char*)picture.GetMemory());
	renderer.SetSampleMode(true);
	renderer.SetFiltering();

	genTextureID = textureID;
	return true;
}

/////////////////////////////////////////////////////////////////////////////
std::vector<std::pair<WString&&, std::shared_ptr<Texture>>> TextureLoader::textures;
TextureLoader::TextureLoader() {
	textures.clear();
}

TextureLoader::~TextureLoader() {
	
}

void TextureLoader::Release() {
	for (auto& tex : textures)
		tex.second->Shutdown();

	textures.clear();
}
const std::shared_ptr<Texture>& TextureLoader::GetTexture(Renderer& renderer, const WString& filename) {
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

bool TextureLoader::Load(Renderer& renderer, const WString& filename) {
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