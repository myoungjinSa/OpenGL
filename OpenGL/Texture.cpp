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
	//FILE* filePtr = 0;
	//int error = fopen_s(&filePtr, fileName.c_str(), "rb");
	//if (error != 0)
	//	return false;

	//TargaHeader targaFileHeader;
	//size_t count = fread(&targaFileHeader, sizeof(TargaHeader), 1, filePtr);
	//if (count != 1)
	//	return false;

	//if (filePtr == 0)
	//	return false;

	//size_t width = (size_t)targaFileHeader.width;
	//size_t height = (size_t)targaFileHeader.height;
	//int bpp = (int)targaFileHeader.bpp;
	//
	////Check that it is 32 bit and not 24 bit.
	//if (bpp != 32)
	//	return false;

	//size_t imageSize = width * height * 4;

	//unsigned char* targaImage = new unsigned char[imageSize];
	//if (!targaImage)
	//	return false;

	////Read in the targa image data
	//count = fread(targaImage, 1, imageSize, filePtr);
	//if (count != imageSize)
	//	return false;

	//if (filePtr == 0)
	//	return false;

	//error = fclose(filePtr);
	//if (error != 0)
	//	return false;
	TargaHeader tgaHeader;
	std::ifstream tgaFile(fileName.c_str(), std::ios::in | std::ios::binary);
	if (!tgaFile.is_open()) {
		std::cout << "Error opening" << fileName.c_str() << std::endl;
		return false;
	}

	//Go to end of file to check TGA Version
	tgaFile.seekg(0, std::ios::end);

	// We need to store the file size for a worst case scenario
	// RLE compression can increase the amount of data
	// depending on the image.
	//(This scenario will only arise, in an image with very few same pixel runs.)
	int fileSize = (int)tgaFile.tellg();

	//Seek to version identifier (Always specified as being 18)
	//characters from the end of the file)
	tgaFile.seekg(-18, std::ios::end);

	char versionCheck[17] = "";
	tgaFile.read(versionCheck, 16);

	int version = 1;
	if (strcmp(versionCheck, "TRUEVISION_XFILE") == 0)
		version = 2;


	//Back to the beginning of the file
	tgaFile.seekg(0, std::ios::beg);

	//Read Header
	tgaFile.read(&tgaHeader.idLength,				sizeof(tgaHeader.idLength));
	tgaFile.read(&tgaHeader.colorMapType,			sizeof(tgaHeader.colorMapType));
	tgaFile.read(&tgaHeader.imageType,				sizeof(tgaHeader.imageType));

	//If colorMatpType is 0 and these 3 fields below are not 0, something may have went wrong.
	tgaFile.read((char*)(&tgaHeader.firstEntry),	sizeof(tgaHeader.firstEntry));
	tgaFile.read((char*)(&tgaHeader.numEntries),	sizeof(tgaHeader.numEntries));
	tgaFile.read(&tgaHeader.bitsPerEntry,			sizeof(tgaHeader.bitsPerEntry));


	tgaFile.read((char*)(&tgaHeader.xOrigin),		sizeof(tgaHeader.xOrigin));
	tgaFile.read((char*)(&tgaHeader.yOrigin),		sizeof(tgaHeader.yOrigin));
	tgaFile.read((char*)(&tgaHeader.width),			sizeof(tgaHeader.width));
	tgaFile.read((char*)(&tgaHeader.height),		sizeof(tgaHeader.height));
	tgaFile.read(&tgaHeader.bitsPerPixel,			sizeof(tgaHeader.bitsPerPixel));
	tgaFile.read(&tgaHeader.descriptor,				sizeof(tgaHeader.descriptor));

	int width = tgaHeader.width;
	int height = tgaHeader.height;

	int tgaDesc = 0;
	switch (tgaHeader.imageType) {
	case 0:		tgaDesc |= TGA_NO_IMAGE;		break;
	case 1:		tgaDesc |= TGA_MAP;				break;
	case 2:		tgaDesc |= TGA_BW;				break;
	case 9:		tgaDesc |= (TGA_MAP | TGA_RLE);	break;
	case 10:	tgaDesc |= (TGA_RGB | TGA_RLE); break;
	case 11:	tgaDesc |= (TGA_BW | TGA_RLE);	break;
	default:	tgaDesc |= TGA_UNSUPPORTED;		break;
	}

	if ((tgaDesc & TGA_UNSUPPORTED) == 0)
		std::cout << "TGA Format Supported" << std::endl;
	else
	{
		std::cout << "TGA Format Unsupported" << std::endl;
		return false;
	}


	//Skip the ID string
	char* pSkip = nullptr;
	tgaFile.read(pSkip, tgaHeader.idLength);

	//Skip the color map if it doesn't exist
	if (!(tgaDesc & TGA_MAP))
	{
		int colorMapSize = tgaHeader.colorMapType * tgaHeader.numEntries;
		tgaFile.read(pSkip, colorMapSize);
	}

	int imageDataSize = tgaHeader.width * tgaHeader.height * (tgaHeader.bitsPerPixel / 8);
	char* pPixelData = new char[imageDataSize];

	//Read the image data
	int imageDataPosition = (int)tgaFile.tellg();
	tgaFile.read(pPixelData, imageDataSize);

	//RLE Decoding
	if (tgaDesc & TGA_RLE) {
		char* pTempPixelData;
		pTempPixelData = new char[fileSize];

		memcpy(pTempPixelData, pPixelData, fileSize);

		int indexAccum = 0;
		int bytesPerPixel = (tgaHeader.bitsPerPixel / 8);
		int bytesPerPixelRLE = bytesPerPixel + 1;

		// Increments of i are controlled in the for loop because depending
		// on whether or not the packet being checked is run-length encoded 
		// the increment may have to be between bytesPerPixel and 128 (Max size of either packet)
		for (int i = 0; indexAccum < imageDataSize; ) {
			// runCount holds the length of the packet taken from the packet info byte
			// runCount can be a maximum of 127.
			int runCount = (127 & pTempPixelData[i]) + 1;

			//Check the packet info byte for RLE
			//Run-length encoded packet
			if (128 & pTempPixelData[i]) {
				// In an encoded packet, runCount specifies
				// the repititions of the pixel data(up to 127)
				for (int j = 0; j < runCount; j++) {
					for (int k = 1; k < bytesPerPixelRLE; k++) {
						pPixelData[indexAccum++] = pTempPixelData[i + k];
					}
				}

				i += bytesPerPixelRLE;
			}
			else if (!(128 & pTempPixelData[i])) {
				//Raw data packet
				
				//Skip past the packet info byte
				i++;
				
				//In a raw packet, runCount specifies
				//the number of pixels that are to follow(up to 127)
				for (int j = 0; j < runCount; j++) {
					for (int k = 0; k < bytesPerPixel; k++)
						pPixelData[indexAccum++] = pTempPixelData[i + k];

					i += bytesPerPixel;
				}
			}
		}
		delete[] pTempPixelData;
		pTempPixelData = nullptr;
	}

	//Set the unique texture unit in which to store the data
	gl.glActiveTexture(GL_TEXTURE0 + textureUnit);

	//Generate an ID for the texture.
	glGenTextures(1, &textureID);

	//Bind the texture as a 2D texture.
	glBindTexture(GL_TEXTURE_2D, textureID);

	//Load the image data into the texture unit.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_BYTE, pPixelData);

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

	delete[] pPixelData;
	pPixelData = nullptr;

	loaded = true;
	return true;
}


/////////////////////////////////////////////////////////////////////////////
std::vector<std::pair<String&&, std::shared_ptr<Texture>>> TextureLoader::textures;
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

