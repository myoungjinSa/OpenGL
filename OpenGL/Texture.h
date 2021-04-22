#pragma once
#include <stdio.h>
#include <vector>
#include "./String/String.h"

class OpenGL;

class TextureLoader;
class Texture
{
	friend TextureLoader;
private:
	struct TargaHeader {
		unsigned char data[12];
		unsigned short width;
		unsigned short height;
		unsigned char bpp;
		unsigned char data2;
	};

public:
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