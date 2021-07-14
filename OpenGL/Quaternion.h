#pragma once
#include <iostream>
#include <math.h>
#include "Matrix.h"


class Quaternion
{
public:
	Quaternion();
	Quaternion(const Vector3<double>& v, double w);
	Quaternion(const Vector4<double>& v);
	Quaternion(const double* arr);
	Quaternion(double x, double y, double z, double w);


	double GetX() const { return value[0]; }
	double GetY() const { return value[1]; }
	double GetZ() const { return value[2]; }
	double GetW() const { return GetReal(); }

	Vector3<double> GetComplex() const { return Vector3<double>(value[0], value[1], value[2]); }
	void SetComplex(const Vector3<double> complex);
	double GetReal() const { return value[3]; }
	void SetReal(double real) { value[3] = real; }

	Quaternion Conjugate() const { return Quaternion(-GetComplex(), GetReal());}
	
	/**
	 * @brief Computes the inverse of this quaternion.
	 *
	 * @note This is a general inverse.  If you know a priori
	 * that you're using a unit quaternion (i.e., norm() == 1),
	 * it will be significantly faster to use conjugate() instead.
	 *
	 * @return The quaternion q such that q * (*this) == (*this) * q
	 * == [ 0 0 0 1 ]<sup>T</sup>.
	 */
	Quaternion Inverse(void) const {
		return Conjugate() / Normalize();
	}

	double Normalize() const { return sqrt(value[0] * value[0] + value[1] * value[1] + value[2] * value[2] + value[3] * value[3]); }
	
	/**
	 * @brief Quaternion scalar division operator.
	 * @param s A scalar by which to divide all components
	 * of this quaternion.
	 * @return The quaternion (*this) / s.
	 */
	Quaternion operator/(double s) const {
		if (s == 0) std::clog << "Dividing quaternion by 0." << std::endl;
		return Quaternion(GetComplex() / s, GetReal() / s);
	}

	
private:
	double value[4];
};

