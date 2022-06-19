#pragma once
#include <iostream>
#include <math.h>
#include "Matrix.h"
#include "MathUtils.h"

#define M_PI   3.14159265358979323846


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
	Quaternion Conjugate(const Vector4<double>& q) const { return Quaternion(-q.x, -q.y, -q.z, q.w); }

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

	Quaternion Inverse(const Vector4<double>& q) const {
		return Conjugate(q) / Normalize(q);
	}

	
	double Normalize() const { return sqrt(value[0] * value[0] + value[1] * value[1] + value[2] * value[2] + value[3] * value[3]); }
	double Normalize(const Vector4<double>& q) const { return sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w); }
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

	/*
		Returns a matrix representation of this quaternion.

		Specifically this is the matrix such that:

		this->matrix() * q.vector() = (*this) * q for any quaternion q.

		Note that this is not the rotation matrix that may be represented by a unit quaternion
		
	*/

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

	Matrix<double, 4, 4> GetRightMatrix() const {
		double m[16] = {
			GetW(), -GetZ(), GetY(), -GetX(),
			GetZ(), GetW(), -GetX(), -GetY(),
			-GetY(), GetX(), GetW(), -GetZ(),
			GetX(), GetY(), GetZ(), GetW()
		};

		Matrix<double, 4, 4> matrix(m);
		return matrix;
	}



	Vector4<double> GetVector() const {
		return Vector4<double>(value[0], value[1], value[2], value[3]);
	}


	/*
		Computes the rotation matrix represented by a unit quaternion

		note: This does not check that this quaternion is normalized.
		it formulaically returns the matrix, which whill not be a rotation if the quaternion is non-unit.
	*/


	Matrix<double, 3, 3> GetRotationMatrix() const {
		double x = GetX();
		double y = GetY();
		double z = GetZ();
		double w = GetW();
		double m[9] = {
			1 - 2 * y * y - 2 * z * z,	2 * x * y - 2 * z * w,		2 * x * z + 2 * y * w,
			2 * x * y + 2 * z * w,		1 - 2 * x * x - 2 * z * z,	2 * y * z - 2 * x * w,
			2 * x * z - 2 * y * w,		2 * y * z + 2 * x * w,		1 - 2 * x * x - 2 * y * y
		};
		Matrix<double, 3, 3> matrix(m);
		return matrix;
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


	void ScaledAxis(const Vector3<double>& w) {
		double theta = w.GetLength();

		if (theta > 0.0001) {
			double s = sin(theta / 2.0);

			Vector3<double> W(w / theta * s);

			value[0] = W.x;
			value[1] = W.y;
			value[2] = W.z;
			value[3] = cos(theta / 2.0);
		}
		else {
			value[0] = value[1] = value[2] = 0;
			value[3] = 1.0;
		}
	}


	/*
		Returns a vector rotated by this quaternion.

		Functionally equivalent to: (rotationMatrix() * v) or (q * Quaternion(0, v) * q.inverse())

		@Warning : Conjugate() is used instead of Inverse() for better performance, when this quaternion must be normalized.
	*/
	Vector3<double> GetRotatedVector(const Vector3<double>& v) const {
		return (((*this) * Quaternion(v, 0) * Conjugate())).GetComplex();
	}


	/*
		Computes the quaternion that is equivalent to a given euler angle rotation.

		@Param euler vector in order : roll - pitch - yaw
	*/
	//void GetEuler(const Vector3<double>& euler) {
	//	double c1 = cos(euler.z * 0.5);
	//	double c2 = cos(euler.y * 0.5);
	//	double c3 = cos(euler.x * 0.5);

	//	double s1 = sin(euler.z * 0.5);
	//	double s2 = sin(euler.y * 0.5);
	//	double s3 = sin(euler.x * 0.5);

	//	value[0] = c1 * c2 * s3 - s1 * s2 * c3;
	//	value[1] = c1 * s2 * c3 + s1 * c2 * s3;
	//	value[2] = s1 * c2 * c3 - c1 * s2 * s3;
	//	value[3] = c1 * c2 * c3 + s1 * s2 * s3;
	//}

	///////////////////////////////////////////////
	//pitch-yaw-roll in order
	void GetEuler(const Vector3<double>& euler) {
		double c1 = cos(euler.x * 0.5);
		double c2 = cos(euler.y * 0.5);
		double c3 = cos(euler.z * 0.5);

		double s1 = sin(euler.x * 0.5);
		double s2 = sin(euler.y * 0.5);
		double s3 = sin(euler.z * 0.5);

		value[0] = c3 * c2 * s1 + s3 * s2 * c1;
		value[1] = c3 * s2 * c1 - s3 * c2 * s1;
		value[2] = s3 * c2 * c1 + c3 * s2 * s1;
		value[3] = c3 * c2 * c1 - s3 * s2 * s1;
	}

	/*
		Returns an equivalent euler angle representation of this quaternion.
		@return Euler angles in pitch - yaw - roll order. (same roll - pitch - yaw)
	*/
	Vector3<double> GetEuler() const {
		Vector3<double> euler;
		const static double PI_OVER_2 = M_PI * 0.5;
		const static double EPSILON = 1e-10;
		double sqw, sqx, sqy, sqz;

		//quick conversion to Euler angle to give tilt to user
		sqw = value[3] * value[3];
		sqx = value[0] * value[0];
		sqy = value[1] * value[1];
		sqz = value[2] * value[2];

		euler.y = asin(2.0 * (value[3] * value[1] - value[0] * value[2]));
		if (PI_OVER_2 - fabs(euler.y) > EPSILON) {
			euler.z = atan2(2.0 * (value[0] * value[1] + value[3] * value[2]), sqx - sqy - sqz + sqw);
			euler.x = atan2(2.0 * (value[3] * value[0] + value[1] * value[2]), sqw - sqx - sqy + sqz);
		}
		else {
			//compute heading from local 'down' vector
			euler.z = atan2(2 * value[1] * value[2] - 2 * value[0] * value[3], 2 * value[0] * value[2] + 2 * value[1] * value[3]);
			euler.x = 0.0;
		
			//if facing down, reverse yaw
			if (euler.y < 0.0)
				euler.z = M_PI - euler.z;
		}
		return euler;
	}

	Quaternion Slerp(const Quaternion& q1, double t) {
		return Slerp(*this, q1, t);
	}

	static Quaternion Slerp(const Quaternion& q0, const Quaternion& q1, double t) {
		double omega = acos(MathUtils::Clamp(q0.value[0] * q1.value[0] + q0.value[1] * q1.value[1] + q0.value[2] * q1.value[2] + q0.value[3] * q1.value[3], -1, 1));

		if (fabs(omega) < 1e-10) {
			omega = 1e-10;
		}

		double som = sin(omega);
		double st0 = sin((1 - t) * omega) / som;
		double st1 = sin(t * omega) / som;

		return Quaternion(q0.value[0] * st0 + q1.value[0] * st1, q0.value[1] * st0 + q1.value[1] * st1,	q0.value[2] * st0 + q1.value[2] * st1, q0.value[3] * st0 + q1.value[3] * st1);
	}


	//Support for 3D Spatial rotations using quaternions, via qmul(qmul(q, v), qconj(q))
	static Vector4<double> Multiply(const Vector4<double>& left, const Vector4<double>& right);
	static Vector3<double> GetXDirection(const Vector4<double>& qVec);
	static Vector3<double> GetYDirection(const Vector4<double>& qVec);
	static Vector3<double> GetZDirection(const Vector4<double>& qVec);

	static Vector3<double> Rotate(const Vector4<double>& q, const Vector3<double>& v);
private:
	double value[4];
};

