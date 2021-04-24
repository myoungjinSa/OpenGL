#pragma once
#include <fstream>
#include <vector>
#include "./String/String.h"

class OpenGL;

class TextureLoader;
class Texture
{
	friend TextureLoader;
private:
	struct TargaHeader {
		/*unsigned char data[12];
		unsigned short width;
		unsigned short height;
		unsigned char bpp;
		unsigned char data2;*/

		//Length of id string
		char idLength;

		//Image storage info
		char colorMapType;
		char imageType;

		//Color Map
		short firstEntry;
		short numEntries;
		char bitsPerEntry;

		//Image description
		short xOrigin;
		short yOrigin;
		short width;
		short height;
		char bitsPerPixel;
		char descriptor;
	};

public:
	enum {
		TGA_UNSUPPORTED = 1,
		TGA_NO_IMAGE	= 2,
		TGA_MAP			= 4,
		TGA_RGB			= 8,
		TGA_BW			= 16,
		TGA_RLE			= 32
	};

	Texture();
	Texture(const Texture& other) = delete;
	~Texture();

	bool Initialize(const OpenGL& gl, const String& fileName, unsigned int textureUnit, bool wrap);
	void Shutdown();

private:
	bool LoadTarga(const OpenGL& gl, const String& fileName, unsigned int textureUnit, bool wrap);
	
	bool loaded;
	unsigned int textureID;
};


class TextureLoader {
public:
	TextureLoader();
	~TextureLoader();
	
	static const std::shared_ptr<Texture>& GetTexture(const OpenGL& gl, const String& filename);
private:
	static bool Load(const OpenGL& gl, String&& filename);

	static std::vector<std::pair<String&&, std::shared_ptr<Texture>>> textures;
	//std::vector<std::shared_ptr<Texture>> textures;
};