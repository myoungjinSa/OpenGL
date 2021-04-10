#pragma once
class RGBA
{
public:
	RGBA();
	RGBA(float red, float green, float blue);
	RGBA(float red, float green, float blue, float alpha);
	RGBA(unsigned int hexColor);
	~RGBA();

	static inline unsigned char GetRed(unsigned int hexColor);
	static inline unsigned char GetGreen(unsigned int hexColor);
	static inline unsigned char GetBlue(unsigned int hexColor);
	inline RGBA WithoutAlpha();

};

