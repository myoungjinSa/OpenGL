#pragma once
#include "Common.h"
#include "Types.h"
#include "./String/String.h"


class Renderer;
class TextureLoader;
class Texture
{
public:
	Texture();
	Texture(const Texture& other) = delete;
	~Texture();

	bool Initialize(Renderer& renderer, const WString& fileName, unsigned int textureUnit, bool wrap);
	void Shutdown();

	bool IsNull()const { return textureID == 0; }

public:
	unsigned int textureID;
private:
	bool Load(Renderer& renderer, const WString& fileName, unsigned int textureUnit, bool wrap, int& genTextureID);
};


class TextureLoader {
public:
	TextureLoader();
	~TextureLoader();
	
	static const std::shared_ptr<Texture>& GetTexture(Renderer& renderer, const WString& filename);
	static void Release();
private:
	static bool Load(Renderer& renderer, const WString& filename);

	static std::vector<std::pair<WString&&, std::shared_ptr<Texture>>> textures;
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