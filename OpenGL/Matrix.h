#pragma once
#include <initializer_list>
#include <cassert>
#include <cmath>
#include "MathUtils.h"

template<typename T, int m, int n> 
class Matrix
{
public :
	enum {
		rows		= m,
		cols		= n,
		channels	= rows * cols,
		shortdim	= (m < n ? m : n)
	};

	typedef T						value_type;
	typedef Matrix<T, m, n>			mat_type;
	typedef Matrix<T, shortdim, 1>	diag_type;

	Matrix();
	
	explicit Matrix(T v0); //!< 1x1 matrix
	Matrix(T v0, T v1);
	Matrix(T v0, T v1, T v2);
	Matrix(T v0, T v1, T v2, T v3);

	explicit Matrix(const T* values);

	Matrix(std::initializer_list<T> values);

	static Matrix All(T alpha);
	static Matrix Zeros();
	static Matrix Ones();
	static Matrix Eyes();
	static Matrix Diagonal(const diag_type& d);

	T Dot(const Matrix<T, m, n>& v)const;

	double DDot(const Matrix<T, m, n>& v)const;

	//Change the matrix shape
	template<int m1, int n1> Matrix<T, m1, n1> Reshape() const;

	//Extract part of the matrix
	template<int m1, int n1> Matrix<T, m1, n1> GetMinor(int i, int j) const;

	Matrix<T, 1, n> Row(int iRow) const;

	Matrix<T, m, 1> Col(int iCol) const;

	diag_type Diagonal()const;

	Matrix<T, n, m> Transpose() const;

	//Matrix<T, n, m> Inverse(int method, bool* p_is_ok = NULL) const;

	Matrix<T, m, n> Multiply(const Matrix<T, m, n>& a) const;

	Matrix<T, m, n> Divide(const Matrix<T, m, n>& a)const;

	const T& operator()(int i, int j) const;
	T& operator()(int i, int j);

	const T& operator() (int i) const;
	T& operator()(int i);

	const T& operator[](size_t idx) const;
	T& operator[](size_t idx);

	T value[m * n];
};

/*Utility Method*/
//template<typename _Tp, int m> static double determinant(const Matx<_Tp, m, m>& a);
//template<typename T, int m, int n> static double Normalize(const Matrix<T, m, n>& mat);
//template<typename T, int m, int n> static double Normalize(const Matrix<T, m, n>& mat, int normType);

template<typename T, int m, int n> inline
Matrix<T, m, n>::Matrix()
{
	for (int i = 0; i < channels; i++) value[i] = T(0);
}


template<typename T, int m, int n> inline
Matrix<T, m, n>::Matrix(T v0)
{
	value[0] = v0;
	for (int i = 1; i < channels; i++) value[i] = T(0);
}

template<typename T, int m, int n> inline
Matrix<T, m, n>::Matrix(T v0, T v1)
{
	static_assert((channels >= 2), "Matx should have at least 2 elements.");
	value[0] = v0; value[1] = v1;
	for (int i = 2; i < channels; i++) value[i] = T(0);
}

template<typename T, int m, int n> inline
Matrix<T, m, n>::Matrix(T v0, T v1, T v2)
{
	static_assert(channels >= 3, "Matx should have at least 3 elements.");
	value[0] = v0; value[1] = v1; value[2] = v2;
	for (int i = 3; i < channels; i++) value[i] = T(0);
}
template<typename T, int m, int n> inline
Matrix<T, m, n>::Matrix(T v0, T v1, T v2, T v3)
{
	static_assert(channels >= 4, "Matx should have at least 4 elements.");
	value[0] = v0; value[1] = v1; value[2] = v2; value[3] = v3;
	for (int i = 4; i < channels; i++) value[i] = T(0);
}

template<typename T, int m, int n> inline
Matrix<T, m, n>::Matrix(const T* valueues)
{
	for (int i = 0; i < channels; i++) value[i] = valueues[i];
}

template<typename T, int m, int n> inline
Matrix<T, m, n>::Matrix(std::initializer_list<T> list)
{
	assert(list.size() == channels);
	int i = 0;
	for (const auto& elem : list)
	{
		value[i++] = elem;
	}
}


template<typename T, int m, int n> inline
Matrix<T, m, n> Matrix<T, m, n>::All(T alpha) {
	Matrix<T, m, n> mat;
	for (int i = 0; i < m * n; i++)
		mat.value[i] = alpha;
	return mat;
}

template<typename T, int m, int n> inline
Matrix<T, m, n> Matrix<T, m, n>::Zeros() {
	return All(0);
}

template<typename T, int m, int n> inline
Matrix<T, m, n> Matrix<T, m, n>::Ones(){
	return All(1);
}

template<typename T, int m, int n> inline
Matrix<T, m, n> Matrix<T, m, n>::Eyes() {
	Matrix<T, m, n> mat;

	for (int i = 0; i < shortdim; i++) {
		Matrix(i, i) = 1;
	}
	return mat;
}


template<typename T, int m, int n> inline
T Matrix<T, m, n>::Dot(const Matrix<T, m, n>& mat)const {
	T s = 0;
	for (int i = 0; i < channels; i++)
		s += value[i] * mat.value[i];
	return s;
}

template<typename T, int m, int n> inline
double Matrix<T, m, n>::DDot(const Matrix<T, m, n>& mat) const {
	double s = 0;
	for (int i = 0; i < channels; i++)
		s += (double)value[i] * mat.value[i];

	return s;
}

template<typename T, int m, int n> inline
Matrix<T, m, n> Matrix<T, m, n>::Diagonal(const typename Matrix<T, m, n>::diag_type& d) {
	Matrix<T, m, n> mat;
	for (int i = 0; i < shortdim; i++)
		mat(i, i) = d(i, 0);
	return mat;
}

template<typename T, int m, int n> template<int m1, int n1> inline
Matrix<T, m1, n1> Matrix<T, m, n>::Reshape() const {
	static_assert((m1 * n1 == m * n), "Input and Destination matrices must have the same number of elements");
	return (const Matrix<T, m1, n1>&)*this;
}

template<typename T, int m, int n>
template<int m1, int n1> inline
Matrix<T, m1, n1> Matrix<T, m, n>::GetMinor(int i, int j) const {
	assert((0 <= i && i + m1 <= m && 0 <= j && j + n1 <= n));
	Matrix<T, m1, n1> s;
	for (int di = 0; di < m1; di++) {
		for (int dj = 0; dj < n1; dj++)
			s(di, dj) = (*this)(i + di, j + dj);
	}
	return s;
}

template<typename T, int m, int n> inline
Matrix<T, 1, n> Matrix<T, m, n>::Row(int i) const {
	assert((unsigned)i < (unsigned)m);
	return Matrix<T, 1, n>(&value[i * n]);
}

template<typename T, int m, int n> inline
Matrix<T, m, 1> Matrix<T, m, n>::Col(int j) const {
	assert((unsigned)j < (unsigned)n);
	Matrix<T, m, 1> v;
	for (int i = 0; i < m; i++)
		v.value[i] = value[i * n + j];
	return v;
}

template<typename T, int m, int n> inline
typename Matrix<T, m, n>::diag_type Matrix<T, m, n>::Diagonal() const
{
	diag_type d;
	for (int i = 0; i < shortdim; i++)
		d.value[i] = value[i * n + i];
	return d;
}
template<typename T, int m, int n> inline
Matrix<T, n, m> Matrix<T, m, n>::Transpose() const
{
	return Matrix<T, n, m>(*this);
}

//template<typename T, int m, int n> inline
//Matrix<T, n, m> Matrix<T, m, n>::Inverse() const {
//
//
//}

template<typename T, int m, int n> inline
Matrix<T, m, n> Matrix<T, m, n>::Multiply(const Matrix<T, m, n>& a) const
{
	return Matrix<T, m, n>(*this, a);
}


template<typename T, int m, int n> inline
Matrix<T, m, n> Matrix<T, m, n>::Divide(const Matrix<T, m, n>& a) const
{
	return Matrix<T, m, n>(*this, a);
}

template<typename T, int m, int n> inline
const T& Matrix<T, m, n>::operator()(int i, int j) const
{
	assert((unsigned)i < (unsigned)m && (unsigned)j < (unsigned)n);
	return this->value[i * n + j];
}

template<typename T, int m, int n> inline
T& Matrix<T, m, n>::operator ()(int i, int j)
{
	assert((unsigned)i < (unsigned)m && (unsigned)j < (unsigned)n);
	return value[i * n + j];
}

template<typename T, int m, int n> inline
const T& Matrix<T, m, n>::operator ()(int i) const
{
	static_assert(m == 1 || n == 1, "Single index indexation requires matrix to be a column or a row");
	assert((unsigned)i < (unsigned)(m + n - 1));
	return value[i];
}

template<typename T, int m, int n> inline
T& Matrix<T, m, n>::operator ()(int i)
{
	static_assert(m == 1 || n == 1, "Single index indexation requires matrix to be a column or a row");
	assert((unsigned)i < (unsigned)(m + n - 1));
	return value[i];
}


template<typename T, int m, int n> inline
const T& Matrix<T, m, n>::operator[](size_t idx) const{
	assert((unsigned)idx < (unsigned) m * n);
	return this->value[idx];
}

template<typename T, int m, int n> inline
T& Matrix<T, m, n>::operator[](size_t idx) {
	assert((unsigned)idx < (unsigned)m * n);
	return this->value[idx];
}

////////////////////////////////////////////////////////////////////
//Vector
template<typename T>
class Vector2 {
public:
	typedef T value_type;
	
	Vector2();
	Vector2(T v0, T v1);
	Vector2(const Vector2<T>& v) = default;
	void SetXY(T x, T y);

	T CalculateMagnitude();
	float CalculateTheta();
	void Normalize();
	Vector2 Multiply(const Vector2<T>& v);	
	static Vector2 GetMidPoint(const Vector2<T>& start, const Vector2<T>& end);


	template<typename T2> operator Vector2<T2>() const;
	Vector2<T>& operator+=(const Vector2<T>& rhs);
	Vector2<T>& operator-=(const Vector2<T>& rhs);
	Vector2<T>& operator*=(const Vector2<T>& rhs);
	Vector2<T>& operator*=(const float& scalarConstant);
	Vector2<T>& operator/=(const float& scalarConstant);


	static const Vector2 ZERO;
	static const Vector2 ONE;
	static const Vector2 UNIT_X;
	static const Vector2 UNIT_Y;

	T x; T y;
};

template<typename T> inline
Vector2<T>::Vector2() 
	: x(T(0)), y(T(0))
{

}

template<typename T> inline
Vector2<T>::Vector2(T v0, T v1)
	: x(v0), y(v1)
{

}

template<typename T> inline
T Vector2<T>::CalculateMagnitude() {
	return sqrt(x * x) + (y * y);
}

template<typename T> inline
void Vector2<T>::Normalize() {
	T len = CalculateMagnitude();
	if (len == T(0))
		return;
	x /= len;
	y /= len;
}

template<typename T> inline
float Vector2<T>::CalculateTheta() {
	return MathUtils::RadiansToDegrees(atan2(y, x));
}

template<typename T> inline
Vector2<T> Vector2<T>::Multiply(const Vector2<T>& v) {
	Vector2<T> ret;
	ret.x = x * v.x;
	ret.y = y * v.y;
	return ret;
}

template<typename T> inline
void Vector2<T>::SetXY(T _x, T _y) {
	x = _x; y = _y; 
}

///////////////////////////////////
//operator
template<typename T> inline
Vector2<T>& Vector2<T>::operator+=(const Vector2<T>& rhs) {
	x += rhs.x;
	y += rhs.y;
	return *this;
}

template<typename T> inline
Vector2<T>& Vector2<T>::operator-=(const Vector2<T>& rhs) {
	x -= rhs.x;
	y -= rhs.y;
	return *this;
}
template<typename T> inline
Vector2<T>& Vector2<T>::operator*=(const Vector2<T>& rhs) {
	x *= rhs.x;
	y *= rhs.y;
	return *this;
}
template<typename T> inline
Vector2<T>& Vector2<T>::operator*=(const float& scalarConstant) {
	x *= scalarConstant;
	y *= scalarConstant;
	return *this;
}

template<typename T> inline
Vector2<T>& Vector2<T>::operator/=(const float& scalarConstant) {
	if (scalarConstant == 0)
		return *this;
	x /= scalarConstant;
	y /= scalarConstant;
	return *this;
}

///////////////////////////////////
//Global Function
template<typename T, typename std::enable_if<std::is_floating_point<T>::value>::type* = nullptr>
Vector2<T> Normalize(const Vector2<T>& v) {
	Vector2<T> ret;
	T length = sqrt(v.x * v.x) + (v.y * v.y);
	ret.x = v.x / length;
	ret.y = v.y / length;
	
	return ret;
}

template<typename T, typename std::enable_if<std::is_floating_point<T>::value>::type* = nullptr>
T Dot(const Vector2<T>& v1, const Vector2<T>& v2) {
	return T((v1.x * v2.x) + (v1.y * v2.y));
}

template<typename T> template<typename T2> inline
Vector2<T>::operator Vector2<T2>() const
{
	Vector2<T2> v;
	v.x = x; v.y = y;;
	return v;
}


template<typename T>
const Vector2<T> Vector2<T>::ZERO(0, 0);

template<typename T>
const Vector2<T> Vector2<T>::ONE(1, 1);

template<typename T>
const Vector2<T> Vector2<T>::UNIT_X(1, 0);

template<typename T>
const Vector2<T> Vector2<T>::UNIT_Y(0, 1);

typedef Vector2<unsigned char> Vec2b;
typedef Vector2<short> Vec2s;
typedef Vector2<unsigned short> Vec2w;
typedef Vector2<int> Vec2i;
typedef Vector2<double> Vec2d;
typedef Vector2<float> Vec2f;

///////
//Global Functions
template<typename T>
Vector2<T> operator+(const Vector2<T>& v1, const Vector2<T>& v2) {
	Vector2<T> ret;

	ret.x = v1.x + v2.x;
	ret.y = v1.y + v2.y;
	return ret;
}

template<typename T>
Vector2<T> operator-(const Vector2<T>& v1, const Vector2<T>& v2) {
	Vector2<T> ret;

	ret.x = v1.x - v2.x;
	ret.y = v1.y - v2.y;
	return ret;
}
template<typename T>
Vector2<T> operator*(const Vector2<T>& v1, const Vector2<T>& v2) {
	Vector2<T> ret;

	ret.x = v1.x * v2.x;
	ret.y = v1.y * v2.y;
	return ret;
}


template<typename T>
Vector2<T> operator*(const Vector2<T>& v1, const float& scalar) {
	Vector2<T> ret;

	ret.x = v1.x * scalar;
	ret.y = v1.y * scalar;
	return ret;
}



template<typename T>
class Vector3{
public:
	typedef T value_type;

	Vector3();
	Vector3(T v0, T v1, T v2);
	//explicit Vector3(const T* values);
	Vector3(const Vector3<T>& v) = default;
	void SetXYZ(T x, T y, T z);

	Vector3 Cross(const Vector3& v) const;
	void Normalize();
	T CalculateMagnitude() const;
	static Vector3 GetMidPoint(const Vector3& start, const Vector3& end);
	Vector3 Multiply(const Vector3<T>& v);

	T* ConvertToValue();

	template<typename T2> operator Vector3<T2>() const;
	Vector3& operator+=(const Vector3& rhs);
	Vector3& operator-=(const Vector3& rhs);
	Vector3& operator*=(const float& scalarConstant);
	Vector3& operator=(const Vector2<T>& rhs);

	static const Vector3 ZERO;
	static const Vector3 ONE;
	static const Vector3 FORWARD;
	static const Vector3 UP;
	static const Vector3 RIGHT;

	T x; T y; T z;
};
template<typename T> inline
Vector3<T>::Vector3()
	: x(T(0)), y(T(0)), z(T(0)) {

}
template<typename T> inline
Vector3<T>::Vector3(T v0, T v1, T v2)
	: x(v0), y(v1), z(v2) {

}

template<typename T> inline
Vector3<T> Vector3<T>::Multiply(const Vector3<T>& v) {
	Vector3<T> ret;
	ret.x = x * v.x;
	ret.y = y * v.y;
	ret.z = z * v.z;
	return ret;
}

template<typename T> template<typename T2> inline
Vector3<T>::operator Vector3<T2>() const
{
	Vector3<T2> v;
	v.x = x; v.y = y; v.z = z;
	return v;
}
template<typename T> inline
Vector3<T> Vector3<T>::Cross(const Vector3<T>& v) const {
	return Vector3<T>(y * z - z * y, z * x - x * z, x * y - y * x);
}

template<typename T>
void Vector3<T>::Normalize() {
	T len = CalculateMagnitude();
	if (len == T(0))
		return;
	x /= len;
	y /= len;
	z /= len;
}
template<typename T> inline
T* Vector3<T>::ConvertToValue() {
	T val[3];
	val[0] = x;
	val[1] = y;
	val[2] = z;

	return *val;
}

template<typename T> 
T Vector3<T>::CalculateMagnitude() const {
	return sqrt((x * x) + (y * y) + (z * z));
}

template<typename T> inline
void Vector3<T>::SetXYZ(T _x, T _y, T _z) {
	x = _x; y = _y; z = _z;
}


/// <summary>
/// operator
/// </summary>
template<typename T> inline
Vector3<T>& Vector3<T>::operator+=(const Vector3<T>& rhs) {
	x += rhs.x;
	y += rhs.y;
	z += rhs.z;
	return *this;
}

template<typename T> inline
Vector3<T>& Vector3<T>::operator-=(const Vector3<T>& rhs) {
	x -= rhs.x;
	y -= rhs.y;
	z -= rhs.z;
	return *this;
}

template<typename T> inline
Vector3<T>& Vector3<T>::operator*=(const float& scalar) {
	x *= scalar;
	y *= scalar;
	z *= scalar;
	return *this;
}

template<typename T> inline
Vector3<T>& Vector3<T>::operator=(const Vector2<T>& rhs) {
	x += rhs.x;
	y += rhs.y;
	z += T(0);
	return *this;
}


template<typename T>
const Vector3<T> Vector3<T>::ZERO(0, 0, 0);

template<typename T>
const Vector3<T> Vector3<T>::ONE(1, 1, 1);

template<typename T>
const Vector3<T> Vector3<T>::FORWARD(0, 0, 1);

template<typename T>
const Vector3<T> Vector3<T>::UP(0, 1, 0);

template<typename T>
const Vector3<T> Vector3<T>::RIGHT(1, 0, 0);

typedef Vector3<unsigned char> Vec3b;
typedef Vector3<short> Vec3s;
typedef Vector3<unsigned short> Vec3w;
typedef Vector3<int> Vec3i;
typedef Vector3<double> Vec3d;
typedef Vector3<float> Vec3f;


///////
//Global Functions
template<typename T>
Vector3<T> operator+(const Vector3<T>& v1, const Vector3<T>& v2) {
	Vector3<T> ret;

	ret.x = v1.x + v2.x;
	ret.y = v1.y + v2.y;
	ret.z = v1.z + v2.z;
	return ret;
}

template<typename T>
Vector3<T> operator-(const Vector3<T>& v1, const Vector3<T>& v2) {
	Vector3<T> ret;
	
	ret.x = v1.x - v2.x;
	ret.y = v1.y - v2.y;
	ret.z = v1.z - v2.z;
	return ret;
}
template<typename T>
Vector3<T> operator*(const Vector3<T>& v1, const Vector3<T>& v2) {
	Vector3<T> ret;

	ret.x = v1.x * v2.x;
	ret.y = v1.y * v2.y;
	ret.z = v1.z * v2.z;
	return ret;
}


template<typename T>
Vector3<T> operator*(const Vector3<T>& v1, const float& scalar) {
	Vector3<T> ret;

	ret.x = v1.x * scalar;
	ret.y = v1.y * scalar;
	ret.z = v1.z * scalar;
	return ret;
}


template<typename T, typename std::enable_if<std::is_floating_point<T>::value>::type* = nullptr>
Vector3<T> Cross(const Vector3<T>& v1, const Vector3<T>& v2) {
	return Vector3<T>(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x);
}

template<typename T, typename std::enable_if<std::is_floating_point<T>::value>::type* = nullptr>
Vector3<T> Normalize(const Vector3<T>& v) {
	Vector3<T> ret;
	T length = sqrt(v.x * v.x) + (v.y * v.y) + (v.z * v.z);
	ret.x = v.x / length;
	ret.y = v.y / length;
	ret.z = v.z / length;
	
	return ret;
}

template<typename T, typename std::enable_if<std::is_floating_point<T>::value>::type* = nullptr>
T Dot(const Vector3<T>& v1, const Vector3<T>& v2) {
	return T((v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z));
}


template<typename T> 
class Vector4 {
public:
	typedef T value_type;

	Vector4();
	Vector4(T v0, T v1, T v2, T v3);
	Vector4(const Vector2<T>& base, float z, float w);
	Vector4(const Vector3<T>& base, float w);
	Vector4(const Vector4<T>& other) = default;
	void SetXYZW(T x, T y, T z, T w);

	T* ConvertToValue();

	///////////////////////////////////////////////
	T CalculateMagnitude() const;
	void Normalize();
	static Vector4 GetMidPoint(const Vector4& start, const Vector4& end);

	
	///////////////////////////////////////////////
	Vector4& operator+=(const Vector4& rhs);
	Vector4& operator-=(const Vector4& rhs);
	Vector4& operator*=(const float& scalarConstant);
	Vector4& operator=(const Vector2<T>& rhs);
	Vector4& operator=(const Vector3<T>& rhs);

	T x; T y; T z; T w;
};

template<typename T> inline
Vector4<T>::Vector4() 
	:x(T(0)), y(T(0)), z(T(0)), w(T(0))
{

}

template<typename T> inline
Vector4<T>::Vector4(T v0, T v1, T v2, T v3) 
	:x(v0), y(v1), z(v2), w(v3)
{

}

template<typename T> inline
Vector4<T>::Vector4(const Vector2<T>& base, float _z, float _w) 
	:x(base.x), y(base.y), z(_z), w(_w)
{

}

template<typename T> inline
Vector4<T>::Vector4(const Vector3<T>& base, float _w) 
	: x(base.x), y(base.y), z(base.z), w(_w)
{

}

template<typename T> inline
T* Vector4<T>::ConvertToValue() {
	T* val;
	val[0] = x;
	val[1] = y;
	val[2] = z;
	val[3] = w;
	return *val;
}

template<typename T> inline
void Vector4<T>::SetXYZW(T _x, T _y, T _z, T _w) {
	x = _x; y = _y; z = _z; w = _w;
}

template<typename T> inline
T Vector4<T>::CalculateMagnitude() const
{
	return sqrt((x * x) + (y * y) + (z * z) + (w * w));
}

template<typename T> inline
void Vector4<T>::Normalize()
{
	T length = CalculateMagnitude();
	if (length == T(0))
	{
		return;
	}
	x /= length;
	y /= length;
	z /= length;
	w /= length;
}


/////////////////////////////////////////////////////////////////////////////////
//operator
template<typename T> inline
Vector4<T>& Vector4<T>::operator+=(const Vector4<T>& rhs)
{
	this->x += rhs.x;
	this->y += rhs.y;
	this->z += rhs.z;
	this->w += rhs.w;
	return *this;
}

template<typename T> inline
Vector4<T>& Vector4<T>::operator-=(const Vector4<T>& rhs)
{
	this->x += rhs.x;
	this->y += rhs.y;
	this->z += rhs.z;
	this->w += rhs.w;
	return *this;
}

template<typename T> inline
Vector4<T>& Vector4<T>::operator*=(const float& scalar)
{
	this->x *= scalar;
	this->y *= scalar;
	this->z *= scalar;
	this->w *= scalar;
	return *this;
}

template<typename T> inline
Vector4<T>& Vector4<T>::operator=(const Vector2<T>& rhs) {
	this->x = rhs.x;
	this->y = rhs.y;
	this->z = 0.0f;
	this->w = 0.0f;
	return *this;
}
template<typename T> inline
Vector4<T>& Vector4<T>::operator=(const Vector3<T>& rhs) {
	this->x = rhs.x;
	this->y = rhs.y;
	this->z = rhs.z;
	this->w = 0.0f;
	return *this;
}



//Global Functions
template<typename T, typename std::enable_if<std::is_floating_point<T>::value>::type* = nullptr>
T Dot(const Vector4<T>& v1, const Vector4<T>& v2) {
	return T((v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z));
}

typedef Vector4<unsigned char> Vec4b;
typedef Vector4<short> Vec4s;
typedef Vector4<unsigned short> Vec4w;
typedef Vector4<int> Vec4i;
typedef Vector4<double> Vec4d;
typedef Vector4<float> Vec4f;