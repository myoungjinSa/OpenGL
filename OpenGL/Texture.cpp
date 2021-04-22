#include "OpenGL.h"
#include "Texture.h"
#include <cassert>

Texture::Texture() 
	: loaded(false), textureID(0)
{

}

Texture::~Texture() {

}

bool Texture::Initialize(const OpenGL& gl, const String& fileName, unsigned int textureUnit, bool wrap) {
	bool result;

	//Load the targa file.
	result = LoadTarga(gl, fileName, textureUnit, wrap);
	if (!result) {
		return false;
	}

	return true;
}

void Texture::Shutdown() {
	if (loaded) {
		glDeleteTextures(1, &textureID);
		loaded = false;
	}
	return;
}

bool Texture::LoadTarga(const OpenGL& gl, const String& fileName, unsigned int textureUnit, bool wrap) {
	FILE* filePtr = 0;
	int error = fopen_s(&filePtr, fileName.c_str(), "rb");
	if (error != 0)
		return false;

	TargaHeader targaFileHeader;
	size_t count = fread(&targaFileHeader, sizeof(TargaHeader), 1, filePtr);
	if (count != 1)
		return false;

	if (filePtr == 0)
		return false;

	size_t width = (size_t)targaFileHeader.width;
	size_t height = (size_t)targaFileHeader.height;
	int bpp = (int)targaFileHeader.bpp;
	
	//Check that it is 32 bit and not 24 bit.
	if (bpp != 32)
		return false;

	size_t imageSize = width * height * 4;

	unsigned char* targaImage = new unsigned char[imageSize];
	if (!targaImage)
		return false;

	//Read in the targa image data
	count = fread(targaImage, 1, imageSize, filePtr);
	if (count != imageSize)
		return false;

	if (filePtr == 0)
		return false;

	error = fclose(filePtr);
	if (error != 0)
		return false;

	//Set the unique texture unit in which to store the data
	gl.glActiveTexture(GL_TEXTURE0 + textureUnit);

	//Generate an ID for the texture.
	glGenTextures(1, &textureID);

	//Bind the texture as a 2D texture.
	glBindTexture(GL_TEXTURE_2D, textureID);

	//Load the image data into the texture unit.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, targaImage);

	if (wrap) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	}

	//Set the texture filtering 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	//Generate mipmaps for the texture
	gl.glGenerateMipmap(GL_TEXTURE_2D);

	delete[] targaImage;
	targaImage = nullptr;

	loaded = true;
	return true;
}


/////////////////////////////////////////////////////////////////////////////
TextureLoader::TextureLoader() {

}

TextureLoader::~TextureLoader() {

}

const std::shared_ptr<Texture>& TextureLoader::GetTexture(const OpenGL& gl, const String& filename) {
	bool bFind = false;
	for (size_t iTexture = 0; iTexture < textures.size(); iTexture++) {
		if (textures[iTexture].first.Compare(filename)) {
			bFind = true;
			return textures[iTexture].second;
		}
	}
	if (!bFind) {
		if (!Load(gl, String(filename))) {
			assert(false);
		}
		bFind = true;
	}
	size_t index = textures.size();
	return textures[index - 1].second;
}

bool TextureLoader::Load(const OpenGL& gl, String&& _filename) {
	std::shared_ptr<Texture> texture = std::make_shared<Texture>();
	if (!texture)
		return false;

	String filename = std::move(_filename);
	if(!texture->Initialize(gl, filename, 0, true))
		return false;

	textures.emplace_back(std::make_pair(filename, texture));
	return true;
}

