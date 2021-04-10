#include "MathUtils.h"
#include <cmath>

const float MathUtils::PI = 3.14159265358f;
const float MathUtils::TWO_PI = 3.14159265358f * 2.0f;
const float MathUtils::HALF_PI = 3.14159265358f / 2.0f;


float MathUtils::Lerp(float fraction, float startValue, float endValue) {
	return startValue + fraction * (endValue - startValue);
}

float MathUtils::DegreesToRadians(float degrees)
{
	return degrees * (PI / 180.0f);
}


float MathUtils::RadiansToDegrees(float radians)
{
	return radians * (180.0f / PI);
}


float MathUtils::CosDegrees(float input)
{
	return cos(DegreesToRadians(input));
}

float MathUtils::SinDegrees(float input)
{
	return sin(DegreesToRadians(input));
}


float MathUtils::RangeMap(float inValue, float min1, float max1, float min2, float max2) {
	if (max1 - min1 == 0)
		return 0.0f;
	return min2 + ((max2 - min2) * (inValue - min1) / (max1 - min1));
}

float MathUtils::Clamp(float inputValue, float min, float max) {
	if (inputValue < min)
	{
		return min;
	}
	else if (inputValue > max)
	{
		return max;
	}
	else
	{
		return inputValue;
	}
}

float MathUtils::Clamp(float inputValue)
{
	return Clamp(inputValue, 0.0f, 1.0f);
}

