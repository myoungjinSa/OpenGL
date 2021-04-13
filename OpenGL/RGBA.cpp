#include "RGBA.h"

const RGBA RGBA::WHITE(0xFFFFFFFF);
const RGBA RGBA::BLACK(0x000000FF);
const RGBA RGBA::RED(0xFF0000FF);
const RGBA RGBA::GREEN(0x00FF00FF);
const RGBA RGBA::BLUE(0x0000FFFF);
const RGBA RGBA::MAGENTA(0xFF00FFFF);
const RGBA RGBA::YELLOW(0xFFFF00FF);
const RGBA RGBA::CYAN(0x00FFFFFF);
const RGBA RGBA::ORANGE(0xFFBB00FF);
const RGBA RGBA::CORNFLOWER_BLUE(0x6495EDFF);
const RGBA RGBA::TURQUOISE(0x00868BFF);
const RGBA RGBA::SLATE(0x2F4F4FFF);
const RGBA RGBA::FOREST_GREEN(0x228B22FF);
const RGBA RGBA::SEA_GREEN(0x43CD80FF);
const RGBA RGBA::KHAKI(0xCDC673FF);
const RGBA RGBA::GOLD(0xCDAD00FF);
const RGBA RGBA::CHOCOLATE(0xD2691EFF);
const RGBA RGBA::SADDLE_BROWN(0x8B4513FF);
const RGBA RGBA::MAROON(0x800000FF);
const RGBA RGBA::GRAY(0x808080FF);
const RGBA RGBA::VAPORWAVE(0xFF819CFF);
const RGBA RGBA::DEADBEEF(0xDEADBEEF);
const RGBA RGBA::FEEDFACE(0xFEEDFACE);
const RGBA RGBA::DISEASED(0xD15EA5ED);
const RGBA RGBA::BADDAD(0xBADDADFF);


RGBA::RGBA() : red(0x00), green(0x00), blue(0x00), alpha(0xFF) {

}

RGBA::RGBA(float red, float green, float blue)
	: red(static_cast<unsigned char>((255.0f * red)))
	, green(static_cast<unsigned char>((255.0f * green)))
	, blue(static_cast<unsigned char>((255.0f * blue)))
	, alpha(0xFF)
{

}

RGBA::RGBA(float red, float green, float blue, float alpha)
	: red(static_cast<unsigned char>((255.0f * red)))
	, green(static_cast<unsigned char>((255.0f * green)))
	, blue(static_cast<unsigned char>((255.0f * blue)))
	, alpha(static_cast<unsigned char>((255.0f * alpha)))
{

}

RGBA::RGBA(unsigned int hexColor)
	: red(GetRed(hexColor))
	, green(GetGreen(hexColor))
	, blue(GetBlue(hexColor))
	, alpha(static_cast<unsigned char>((hexColor & ALPHA_MASK)))
{

}

template<typename T> inline
RGBA::RGBA(const Vector4<T>& color) 
	: red(static_cast<unsigned char>((255.0f * color.x)))
	, green(static_cast<unsigned char>((255.0f * color.y)))
	, blue(static_cast<unsigned char>((255.0f * color.z)))
	, alpha(static_cast<unsigned char>((255.0f * color.w)))
{

}

RGBA::~RGBA()
{
}

template<typename T> inline
Vector3<T> RGBA::ToVec3() const {
	return Vector3<T>(static_cast<T>(red) / T(255), static_cast<T>(green) / T(255), static_cast<T>(blue) / T(255));
}

template<typename T> inline
Vector4<T> RGBA::ToVec4() const {
	return Vector4<T>(static_cast<T>(red) / T(255), static_cast<T>(green) / T(255), static_cast<T>(blue) / T(255), static_cast<T>(alpha) / T(255));
}

