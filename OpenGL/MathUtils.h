#pragma once
#define BIT(X) (1 << (X))

#define MAX(a, b) ((a < b) ? b : a)
#define MIN(a, b) ((a > b) ? b : a)
class MathUtils
{
public:
	static float Lerp(float fraction, float startValue, float endValue);
	static float DegreesToRadians(float degree);
	static float RadiansToDegrees(float radian);

	static float CosDegrees(float input);
	static float SinDegrees(float input);

	static float RangeMap(float inValue, float min1, float max1, float min2, float max2);
	static float Clamp(float inputValue);
	static float Clamp(float inputValue, float min, float max);
	static int Clamp(int inputValue, int min, int max);
	static double Clamp(double inputValue, int min, int max);

	static double Saturate(double v);

	static bool IsSame(double left, double right);
	static bool IsSame(float left, float right);

	static const float PI;
	static const float TWO_PI;
	static const float HALF_PI;
};

