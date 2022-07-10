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
	ret.MultiplyAfter(mat1);
	ret.MultiplyAfter(mat2);
	return ret;
}


Matrix<float, 3, 3> MathUtils::CalculateRotationMatrix(const Vec3f& _axis, float angle) {
	Vec3f axis = _axis;
	axis.Normalize();

	float cos = std::cos(angle);
	float sin = std::sin(angle);
	float tan = 1 - cos;

	float x = axis.x;
	float y = axis.y;
	float z = axis.z;

	Matrix<float, 3, 3> rotationMatrix = Matrix<float, 3, 3>::Identity(); 

	rotationMatrix.value[0] = tan * x * x + cos;
	rotationMatrix.value[1] = tan * x * y + z * sin;
	rotationMatrix.value[2] = tan * x * z - y * sin;

	rotationMatrix.value[3] = tan * x * y - z * sin;
	rotationMatrix.value[4] = tan * y * y + cos;
	rotationMatrix.value[5] = tan * y * z + x * sin;
	
	rotationMatrix.value[6] = tan * x * z + y * sin;
	rotationMatrix.value[7] = tan * y * z - x * sin;
	rotationMatrix.value[8] = tan * z * z + cos;
	
	return rotationMatrix;
}

Matrix<float, 3, 3> MathUtils::CalculateRotationMatrix(float pitch, float yaw, float roll) {
	Matrix<float, 3, 3> rotationMatrix = Matrix<float, 3, 3>::Identity();
	
	float cYaw, cPitch, cRoll, sYaw, sPitch, sRoll;

	cYaw = cosf(yaw);
	cPitch = cosf(pitch);
	cRoll = cosf(roll);

	sYaw = sinf(yaw);
	sPitch = sinf(pitch);
	sRoll = sinf(roll);

	//Calculate the yaw, pitch , roll rotation matrix.
	rotationMatrix[0] = (cRoll * cYaw) + (sRoll * sPitch * sYaw);
	rotationMatrix[1] = (sRoll * cPitch);
	rotationMatrix[2] = (cRoll * -sYaw) + (sRoll * sPitch * cYaw);

	rotationMatrix[3] = (-sRoll * cYaw) + (cRoll * sPitch * sYaw);
	rotationMatrix[4] = (cRoll * cPitch);
	rotationMatrix[5] = (sRoll * sYaw) + (cRoll * sPitch * cYaw);
	
	rotationMatrix[6] = (cPitch * sYaw);
	rotationMatrix[7] = -sPitch;
	rotationMatrix[8] = (cPitch * cYaw);
	
	return rotationMatrix;
}

Vec4d MathUtils::GetRotatedQuaternion(const Vec3f& axis, float angle) {
	//angle must be radian
	Vec3d _axis = axis * std::sin(angle / 2.0f);
	return Vec4d(_axis, std::cos(angle / 2.0f));
}


