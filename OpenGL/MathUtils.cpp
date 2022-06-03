#include "MathUtils.h"
#include <cmath>
#include <float.h>

const float MathUtils::PI = 3.14159265358f;
const float MathUtils::TWO_PI = 6.28318530718f;
const float MathUtils::HALF_PI = 3.14159265358f / 2.0f;

float MathUtils::Lerp(float a, float b, float t) {
	return (1.0f - t) * a + t * b ;
}

float MathUtils::DegreesToRadians(float degrees)
{
	return degrees * (PI / 180.0f);
}


float MathUtils::RadiansToDegrees(float radians)
{
	return radians * (180.0f / PI);
}


float MathUtils::CosDegrees(float angle)
{
	return cos(DegreesToRadians(angle));
}

float MathUtils::SinDegrees(float angle)
{
	return sin(DegreesToRadians(angle));
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

	return inputValue;
}


float MathUtils::Clamp(float inputValue)
{
	return Clamp(inputValue, 0.0f, 1.0f);
}

double MathUtils::Saturate(double v) {
	if (v < 0.0) return 0.0;
	if (v > 1.0) return 1.0;
	return v;
}
double MathUtils::Clamp(double inputValue, int min, int max) {
	if (inputValue < min)
	{
		return min;
	}
	else if (inputValue > max)
	{
		return max;
	}
	
	return inputValue;
}

bool MathUtils::IsSame(double left, double right) {
	return abs(left - right) < DBL_EPSILON;
}

bool MathUtils::IsSame(float left, float right) {
	return abs(left - right) < FLT_EPSILON;
}

Matrix<float, 4, 4>  MathUtils::Multiply(const Matrix<float, 4, 4>& mat1, const Matrix<float, 4, 4>& mat2) {
	Matrix<float, 4, 4> ret = Matrix<float, 4, 4>::Identity();
	ret.Multiply(mat1);
	ret.Multiply(mat2);
	return ret;
}
