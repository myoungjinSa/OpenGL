#pragma once
#include <stdio.h>

class OpenGL;

class Texture
{
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

	bool Initialize(OpenGL* gl, char* fileName, unsigned int textureUnit, bool wrap);
	void Shutdown();

private:
	bool LoadTarga(OpenGL* gl, char* fileName, unsigned int textureUnit, bool wrap);
	
	bool loaded;
	unsigned int textureID;
};

