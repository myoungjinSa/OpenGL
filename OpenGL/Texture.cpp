#include "Texture.h"
#include "Renderer.h"
#include "Types.h"
#include <fstream>
#include <cassert>
#include "TGAReader/tga_reader.h"
#include "Picture.h"
#include "PictureFile.h"

#include "OpenGL.h"
Texture::Texture() 
	: textureID(0)
	, type(eType::TYPE_COUNT)
{

}

Texture::~Texture() {
	
}

bool Texture::Initialize(Renderer& renderer, const WString& _fileName) {
	bool result;

	renderContext = std::make_unique<RenderContext>(renderer);
	int texID = 0;
	result = Load(_fileName, texID);
	if (!result) {
		return false;
	}

	textureID = (unsigned int)texID;
	//renderer.AllocateTextures(textureID, 1);
	
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

	if (renderContext->preVideoFile) {
		int64_t pts = renderContext->preVideoFile->CalcPts(renderContext->totalDeltaTime);
		PictureFile::UpdateVideoFrame(picture, renderContext->preVideoFile, pts);
	}

	renderContext->renderer.BindTexture(textureID);
	renderContext->renderer.SetImage(GL_TEXTURE_2D, picture.GetMemory(), picture.GetWidth(), picture.GetHeight());
	renderContext->renderer.SetSampleMode(false);
	renderContext->renderer.SetFiltering();
	
	return true;
}
void Texture::Shutdown() {
	renderContext->renderer.DeleteTexture(*this);
	return;
}

bool Texture::Load(const WString& filename, int& genTextureID) {
	if (!picture.IsNull()) {
		picture.Destroy();
	}

	if (!FileUtils::DoesFileExist(filename)) {
		return false;
	}
	
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
	
	genTextureID = textureID;
	return true;
}

/////////////////////////////////////////////////////////////////////////
//CubemapTexture
CubemapTexture::CubemapTexture() 
	: textureID(0)
{
	
}

bool CubemapTexture::LoadTexture(Renderer& renderer, eCubemapSide side, const WString& filename) {
	if (at((size_t)side)) {
		assert(0);
		return false;
	}

	std::shared_ptr<Texture> texture = TextureLoader::GetTexture(renderer, filename);

	at((size_t)side) = texture;

	return true;
}

bool CubemapTexture::Init(Renderer& renderer) {
	renderer.AllocateTextures(textureID, 1);
	renderer.BindCubemapTexture(textureID);
	renderer.SetSampleMode(true);
	renderer.SetFiltering(true);

	for (size_t iTexture = 0; iTexture < size(); iTexture++) {
		Size2u imageSize(at(iTexture)->GetPicture().GetWidth(), at(iTexture)->GetPicture().GetHeight());
		renderer.SetImage(GL_TEXTURE_CUBE_MAP_POSITIVE_X + iTexture, at(iTexture)->GetPicture().GetMemory(), imageSize.width, imageSize.height);
		at(iTexture)->textureID = textureID;
	}

	return textureID == 0 ? false : true;
}



/////////////////////////////////////////////////////////////////////////////
std::vector<std::pair<WString, std::shared_ptr<Texture>>> TextureLoader::textures;
TextureLoader::TextureLoader() {
	textures.clear();
}

TextureLoader::~TextureLoader() {
	
}

void TextureLoader::Release() {
	for (auto& tex : textures) {
		tex.second->Shutdown();
		tex.second.reset();
	}

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

void TextureLoader::ResetTexture(Renderer& renderer, const WString& filename) {
	for (size_t iTexture = 0; iTexture < textures.size(); iTexture++) {
		if (textures[iTexture].first.Compare(filename)) {
			textures[iTexture].second.reset();
		}
	}
}

bool TextureLoader::Load(Renderer& renderer, const WString& filename) {
	std::shared_ptr<Texture> texture = std::make_shared<Texture>();
	if (!texture)
		return false;

	if(!texture->Initialize(renderer, filename))
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