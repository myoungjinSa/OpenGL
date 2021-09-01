#pragma once
#include "Matrix.h"

class RGBA
{
public:
	RGBA();
	RGBA(float red, float green, float blue);
	RGBA(float red, float green, float blue, float alpha);
	RGBA(unsigned int hexColor);
	RGBA(const RGBA& RGBA) = default;

	template<typename T>
	RGBA(const Vector4<T>& color);
	~RGBA();

	static inline unsigned char GetRed(unsigned int hexColor);
	static inline unsigned char GetGreen(unsigned int hexColor);
	static inline unsigned char GetBlue(unsigned int hexColor);
	inline RGBA WithoutAlpha();
	
	template<typename T> Vector3<T> ToVec3() const;
	template<typename T> Vector4<T> ToVec4()const;


	unsigned char red;
	unsigned char green;
	unsigned char blue;
	unsigned char alpha;

	//CONSTANTS//////////////////////////////////////////////////////////////////////////
	static const RGBA WHITE;
	static const RGBA BLACK;
	static const RGBA RED;
	static const RGBA BLUE;
	static const RGBA GREEN;
	static const RGBA MAGENTA;
	static const RGBA YELLOW;
	static const RGBA CYAN;
	static const RGBA ORANGE;
	static const RGBA CORNFLOWER_BLUE;
	static const RGBA TURQUOISE;
	static const RGBA SLATE;
	static const RGBA FOREST_GREEN;
	static const RGBA SEA_GREEN;
	static const RGBA KHAKI;
	static const RGBA GOLD;
	static const RGBA CHOCOLATE;
	static const RGBA SADDLE_BROWN;
	static const RGBA MAROON;
	static const RGBA GRAY;
	static const RGBA VAPORWAVE;
	static const RGBA DEADBEEF;
	static const RGBA FEEDFACE;
	static const RGBA DISEASED;
	static const RGBA BADDAD;
	static const int SHIFT_RED = 6 * 4;
	static const int SHIFT_GREEN = 4 * 4;
	static const int SHIFT_BLUE = 2 * 4;
	static const int RED_MASK = 0xFF000000;
	static const int GREEN_MASK = 0x00FF0000;
	static const int BLUE_MASK = 0x0000FF00;
	static const int ALPHA_MASK = 0x000000FF;
};

inline unsigned char RGBA::GetRed(unsigned int hexColor) {
	return (unsigned char)((hexColor & RED_MASK) >> SHIFT_RED);
}

inline unsigned char RGBA::GetGreen(unsigned int hexColor) {
	return (unsigned char)((hexColor & GREEN_MASK) >> SHIFT_GREEN);
}

inline unsigned char RGBA::GetBlue(unsigned int hexColor) {
	return (unsigned char)((hexColor & BLUE_MASK) >> SHIFT_BLUE);
}


//-----------------------------------------------------------------------------------
inline RGBA RGBA::WithoutAlpha()
{
	return RGBA((red << RGBA::SHIFT_RED) + (green << RGBA::SHIFT_GREEN) + (blue << RGBA::SHIFT_BLUE));
}


//---------------------------------------------------------------------------
//Global Functions

inline bool operator==(const RGBA& lhs, const RGBA& rhs)
{
	return (lhs.red == rhs.red) && (lhs.green == rhs.green) && (lhs.blue == rhs.blue) && (lhs.alpha == rhs.alpha);
}


inline bool operator!=(const RGBA& lhs, const RGBA& rhs)
{
	return !(lhs == rhs);
}


