#pragma once
#include "Types.h"
#define DEFAULT_BPP 32

class PictureFormat {
public:
	static int CalcPitch(size_t imageWidth, int bpp) {
		return (bpp / 8) * imageWidth;
	}
};

class Picture {
	Picture();
	Picture(const Picture& other);
	virtual ~Picture();

	Picture& operator=(const Picture& other);

	bool IsNull() const {	return pImage == nullptr;	}
	void* GetMemory() const { return pImage; }

	int GetWidth() const { return width; }
	int GetHeight() const { return height; }
	Size2u GetSize() const { return Size2u(width, height); }
	int GetDepth() const { return bpp; }
	int GetPitch() const { return pitch; }

	bool Create(void* pData, int width, int height, int bpp);
	void Destroy();
protected:
	void* pImage;
	int   width;
	int   height;
	int   bpp;
	int   pitch;
};