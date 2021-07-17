#pragma once
#include <iostream>
#include <math.h>
#include "Matrix.h"


class Quaternion
{
public:
	Quaternion() {
		value[0] = value[1] = value[2] = 0;
		value[3] = 1;
	}
	Quaternion(const Vector3<double>& v, double w) {
		value[0] = v.x;
		value[1] = v.y;
		value[2] = v.z;
		value[3] = w;
	}
	Quaternion(const Vector4<double>& v) {
		value[0] = v.x;
		value[1] = v.y;
		value[2] = v.z;
		value[3] = v.w;
	}
	Quaternion(const double* arr) {
		if (!arr)
			assert(0);
		for (size_t iVal = 0; iVal < 4; iVal++) {
			value[iVal] = arr[iVal];
		}
	}

	Quaternion(double x, double y, double z, double w) {
		value[0] = x;
		value[1] = y;
		value[2] = z;
		value[3] = w;
	}

	double GetX() const { return value[0]; }
	double GetY() const { return value[1]; }
	double GetZ() const { return value[2]; }
	double GetW() const { return GetReal(); }

	Vector3<double> GetComplex() const { return Vector3<double>(value[0], value[1], value[2]); }
	void SetComplex(const Vector3<double> complex) {
		value[0] = complex.x; value[1] = complex.y; value[2] = complex.z;
	}
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
	* @brief Quaternion product operator.
	*
	* The result is a quaternion such that:
	*
	* result.real() = (*this).real() * rhs.real() -
	* (*this).complex().dot(rhs.complex());
	*
	* and:
	*
	* result.complex() = rhs.complex() * (*this).real
	* + (*this).complex() * rhs.real()
	* - (*this).complex().cross(rhs.complex());
	*
	* @return The quaternion product (*this) x rhs.
	*/
	Quaternion operator*(const Quaternion& rhs) const {
		return Product(rhs);
	}
	/**
	* @brief Quaternion scalar product operator.
	* @param s A scalar by which to multiply all components
	* of this quaternion.
	* @return The quaternion (*this) * s.
	*/
	Quaternion operator*(double s) const {
		return Quaternion(GetComplex() * s, GetReal() * s);
	}

	/**
	* @brief Produces the sum of this quaternion and rhs.
	*/
	Quaternion operator+(const Quaternion& rhs) const {
		return Quaternion(GetX() + rhs.GetX(), GetY() + rhs.GetY(), GetZ() + rhs.GetZ(), GetW() + rhs.GetW());
	}

	/**
	* @brief Produces the difference of this quaternion and rhs.
	*/
	Quaternion operator-(const Quaternion& rhs) const {
		return Quaternion(GetX() - rhs.GetX(), GetY() - rhs.GetY(), GetZ() - rhs.GetZ(), GetW() - rhs.GetW());
	}
	/**
	 * @brief Unary negation.
	 */
	Quaternion operator-() const {
		return Quaternion(-GetX(), -GetY(), -GetZ(), -GetW());
	}
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

	Matrix<double, 4, 4> GetMatrix() const {
		double m[16] = {
			GetW(), -GetZ(), GetY(), GetX(),
			GetZ(), GetW(), -GetX(), GetY(),
			-GetY(), GetX(), GetW(), GetZ(),
			-GetX(), -GetY(), -GetZ(), GetW()
		};
		Matrix<double, 4, 4> matrix(m);
		return matrix;
	}

	Vector4<double> GetVector() const {
		return Vector4<double>(value[0], value[1], value[2], value[3]);
	}

	/**
	* @brief Computes the product of this quaternion with the
	* quaternion 'rhs'.
	*
	* @param rhs The right-hand-side of the product operation.
	*
	* @return The quaternion product (*this) x @p rhs.
	*/
	Quaternion Product(const Quaternion& rhs) const {
		return Quaternion(GetY() * rhs.GetZ() - GetZ() * rhs.GetY() + GetX() * rhs.GetW() + GetW() * rhs.GetX(),
						  GetZ() * rhs.GetX() - GetX() * rhs.GetZ() + GetY() * rhs.GetW() + GetW() * rhs.GetY(),
						  GetX() * rhs.GetY() - GetY() * rhs.GetX() + GetZ() * rhs.GetW() + GetW() * rhs.GetZ(),
						  GetW() * rhs.GetW() - GetX() * rhs.GetX() - GetY() * rhs.GetY() - GetZ() * rhs.GetZ());
	}



private:
	double value[4];
};

