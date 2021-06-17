#pragma once
#include "Common.h"
#include "Types.h"
#include "./String/String.h"


class Renderer;
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

	bool Initialize(Renderer& renderer, const String& fileName, unsigned int textureUnit, bool wrap);
	void Shutdown();

	bool IsNull()const { return textureID == 0; }

public:
	unsigned int textureID;
private:
	bool LoadBMP(Renderer& renderer, const String& fileName, unsigned int textureUnit, bool wrap, int& genTextureID);

	bool loaded;
};


class TextureLoader {
public:
	TextureLoader();
	~TextureLoader();
	
	static const std::shared_ptr<Texture>& GetTexture(Renderer& renderer, const String& filename);
	static void Release();
private:
	static bool Load(Renderer& renderer, const String& filename);

	static std::vector<std::pair<String&&, std::shared_ptr<Texture>>> textures;
};

class RenderTarget {
public:
	RenderTarget();
	~RenderTarget();

	bool Create(Renderer& renderer, size_t fboCount);
	
	void SetSize(const Size2u& _size);
	void SetSize(size_t width, size_t height);
	Size2u GetSize() const;

	bool IsNull() const { return iFrameBuffer == 0; }

public:
	unsigned int iFrameBuffer;

private:
	Size2u size;
};