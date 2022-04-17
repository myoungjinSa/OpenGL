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

	static Matrix Identity();
	static Matrix All(T alpha);
	static Matrix Zeros();
	static Matrix Ones();
	static Matrix Eyes();
	static Matrix Diagonal(const diag_type& d);

	T DotProduct(const Matrix<T, m, n>& v)const;

	double DDotProduct(const Matrix<T, m, n>& v)const;

	//Change the matrix shape
	template<int m1, int n1> Matrix<T, m1, n1> Reshape() const;

	//Extract part of the matrix
	template<int m1, int n1> Matrix<T, m1, n1> GetMinor(int i, int j) const;

	Matrix<T, 1, n> Row(int iRow) const;

	Matrix<T, m, 1> Col(int iCol) const;

	diag_type Diagonal()const;

	Matrix<T, n, m> Transpose() const;

	void Multiply(const Matrix<T, m, n>& a);

	Matrix<T, m, n> Divide(const Matrix<T, m, n>& a)const;

	const T& operator()(int i, int j) const;
	T& operator()(int i, int j);

	const T& operator() (int i) const;
	T& operator()(int i);

	const T& operator[](size_t idx) const;
	T& operator[](size_t idx);

	void operator*(T val) const;
	Matrix<T, m, n>& operator/(T val);

	
	T value[m * n];
};


Matrix<float, 2, 2> GetCofactor(const Matrix<float, 3, 3>& mat, int iRow, int iCol);
Matrix<float, 3, 3> GetCofactor(const Matrix<float, 4, 4>& mat, int iRow, int iCol);
Matrix<float, 4, 4> GetAdjMatrix(const Matrix<float, 4, 4>& mat);

double GetDeterminant(const Matrix<float, 2, 2>& other);
double GetDeterminant(const Matrix<float, 3, 3>& other);
double GetDeterminant(const Matrix<float, 4, 4>& other);

Matrix<float, 2, 2> Inverse(const Matrix<float, 2, 2>& mat);
Matrix<float, 3, 3> Inverse(const Matrix<float, 3, 3>& mat);
Matrix<float, 4, 4> Inverse(const Matrix<float, 4, 4>& mat);

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
Matrix<T, m, n> Matrix<T, m, n>::Identity() {
	Matrix<T, m, n> mat = Matrix::Zeros();

	for (int iRow = 0; iRow < m; iRow++) {
		for (int iCol = 0; iCol < n; iCol++) {
			if (iRow == iCol)
				mat.value[iRow * m + iCol] = 1;
		}
	}
	return mat;
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
T Matrix<T, m, n>::DotProduct(const Matrix<T, m, n>& mat)const {
	T s = 0;
	for (int i = 0; i < channels; i++)
		s += value[i] * mat.value[i];
	return s;
}

template<typename T, int m, int n> inline
double Matrix<T, m, n>::DDotProduct(const Matrix<T, m, n>& mat) const {
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
	return Matrix<T, 1, n>(value[i * n]);
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
	Matrix<T, n, m> transposeMatrix;
	for (int iRow = 0; iRow < rows; iRow++) {
		for (int iCol = 0; iCol < cols; iCol++) {
			transposeMatrix[iCol * m + iRow] = value[iRow * n + iCol];
		}
	}
	
	return transposeMatrix;
}

template<typename T, int m, int n> inline
void Matrix<T, m, n>::Multiply(const Matrix<T, m, n>& a)
{
	assert((unsigned)n == (unsigned)m);
	Matrix<float, 4, 4> tempMatrix = Matrix<float, 4, 4>::Identity();
	for (int i = 0; i < rows * cols; i++) {
		tempMatrix.value[i] = value[i];
	}

	T sum = T(0);
	for (int iRow = 0; iRow < m * rows; iRow += m) {
		for (int iCol = 0; iCol < cols; iCol ++) {
			for (int k = 0; k < cols; k++) {
				sum += tempMatrix.value[iRow + k] * a.value[cols * k + iCol];
			}
			value[iRow + iCol] = sum;
			sum = 0;
		}
	}

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

template<typename T, int m, int n> inline
void Matrix<T, m, n>::operator*(T val) const{
	for (size_t iVal = 0; iVal < rows * cols; iVal++) {
		this->value[iVal] *= val;
	}
}

template<typename T, int m, int n> inline
Matrix<T, m, n>& Matrix<T, m, n>::operator/(T val) {
	if (val == T(0))
		return *this;

	for (size_t iVal = 0; iVal < rows * cols; iVal++) {
		this->value[iVal] /= val;
	}

	return *this;
}

////////////////////////////////////////////////////////////////////
//Vector
template<typename T>
class Vector2 {
public:
	typedef T value_type;
	
	Vector2();
	Vector2(T v0, T v1);
	Vector2(T s);
	Vector2(const Vector2<T>& v) = default;
	void SetXY(T x, T y);
	bool IsZero() const;

	T GetLength();
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
Vector2<T>::Vector2(T s) 
	: Vector2<T>(s, s)
{

}

template<typename T> inline
T Vector2<T>::GetLength() {
	return sqrt(x * x) + (y * y);
}

template<typename T> inline
void Vector2<T>::Normalize() {
	T len = GetLength();
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

template<typename T> inline
bool Vector2<T>::IsZero() const{
	return x == T(0) && y == T(0);
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

template<typename T>
T DotProduct(const Vector2<T>& v1, const Vector2<T>& v2) {
	return T((v1.x * v2.x) + (v1.y * v2.y));
}
template<class T>
T Length(const Vector2<T>& a)
{
	return DotProduct(a, a);
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
	Vector3(T s);
	//explicit Vector3(const T* values);
	Vector3(const Matrix<T, 1, 3>& mat);
	Vector3(const Vector3<T>& v) = default;
	void SetXYZ(T x, T y, T z);
	bool IsZero() const;

	Vector3 Cross(const Vector3& v) const;
	T DotProduct(const Vector3& v)const;
	void Normalize();
	T GetLength() const;
	static Vector3 GetMidPoint(const Vector3& start, const Vector3& end);
	Vector3 Multiply(const Vector3<T>& v);

	T* ConvertToValue();

	template<typename T2> operator Vector3<T2>() const;
	Vector3& operator+=(const Vector3& rhs);
	Vector3& operator-=(const Vector3& rhs);
	Vector3& operator/=(const Vector3& rhs);
	Vector3& operator*=(const float& scalarConstant);
	Vector3& operator=(const Vector2<T>& rhs);

	Vector3 operator-()const;
	Vector3 operator/(double d) const;

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
Vector3<T>::Vector3(T s) 
	: Vector3<T>(s, s, s)
{

}


template<typename T> inline
Vector3<T>::Vector3(const Matrix<T, 1, 3>& mat) {
	x = mat.Row(0).Col(0);
	y = mat.Row(0).Col(1);
	z = mat.Row(0).Col(2);
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
	return Vector3<T>(v.y * z - v.z * y, v.z * x - v.x * z, v.x * y - v.y * x);
}

template<typename T> inline
T Vector3<T>::DotProduct(const Vector3<T>& v) const {
	return T((x * v.x) + (y * v.y) + (z * v.z));
}

template<typename T>
void Vector3<T>::Normalize() {
	T len = GetLength();
	if (len == T(0))
		return;
	x /= len;
	y /= len;
	z /= len;
}
template<typename T> inline
T* Vector3<T>::ConvertToValue() {
	T val[3] = {0, };
	val[0] = x;
	val[1] = y;
	val[2] = z;

	return val;
}

template<typename T> 
T Vector3<T>::GetLength() const {
	return sqrt((x * x) + (y * y) + (z * z));
}

template<typename T> inline
void Vector3<T>::SetXYZ(T _x, T _y, T _z) {
	x = _x; y = _y; z = _z;
}

template<typename T> inline
bool Vector3<T>::IsZero() const {
	return x == T(0) && y == T(0) && z == T(0);
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
Vector3<T>& Vector3<T>::operator/=(const Vector3<T>& rhs) {
	x /= rhs.x;
	y /= rhs.y;
	z /= rhs.z;
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

template<typename T> inline
Vector3<T> Vector3<T>::operator-() const {
	return Vector3<T>(-x, -y, -z);
}
template<typename T> inline
Vector3<T> Vector3<T>::operator/(double d) const {
	//static_assert((d == 0), "dividng by 0.");
	return Vector3<T>(x / d, y / d, z / d);
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
Vector3<T> operator+(const Vector3<T>& v1,  float value) {
	return Vector3<T>(v1.x + value, v1.y + value, v1.z + value);
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
Vector3<T> operator-(const Vector3<T>& v1, float value) {
	return Vector3<T>(v1.x - value, v1.y - value, v1.z - value);
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
template<typename T>
Vector3<T> operator/(const Vector3<T>& v1, const Vector3<T>& v2) {
	Vector3<T> ret;
	assert(v2.x != 0.0 && v2.y != 0.0 && v2.z != 0.0);

	ret.x = v1.x / v2.x;
	ret.y = v1.y / v2.y;
	ret.z = v1.z / v2.z;
	return ret;
}

template<typename T, typename std::enable_if<std::is_floating_point<T>::value>::type* = nullptr>
Vector3<T> Cross(const Vector3<T>& v1, const Vector3<T>& v2) {
	return Vector3<T>(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x);
}

template<typename T, typename std::enable_if<std::is_floating_point<T>::value>::type* = nullptr>
Vector3<T> Normalize(const Vector3<T>& v) {
	Vector3<T> ret;
	T length = sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
	ret.x = v.x / length;
	ret.y = v.y / length;
	ret.z = v.z / length;
	
	return ret;
}

template<typename T>
T DotProduct(const Vector3<T>& v1, const Vector3<T>& v2) {
	return T((v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z));
}

template<class T> 
T Length(const Vector3<T>& a) 
{ 
	return DotProduct(a, a); 
}


template<typename T> 
class Vector4 {
public:
	typedef T value_type;

	Vector4();
	Vector4(T v0, T v1, T v2, T v3);
	Vector4(T s);
	Vector4(const Vector2<T>& base, float z, float w);
	Vector4(const Vector3<T>& base, float w);
	Vector4(const Vector4<T>& other) = default;
	Vector4(const Matrix<T, 1, 4>& mat);
	void SetXYZW(T x, T y, T z, T w);
	bool IsZero() const;

	T* ConvertToValue();

	///////////////////////////////////////////////
	T GetLength() const;
	void Normalize();
	static Vector4 GetMidPoint(const Vector4& start, const Vector4& end);

	
	///////////////////////////////////////////////
	Vector4& operator+=(const Vector4& rhs);
	Vector4& operator-=(const Vector4& rhs);
	Vector4& operator*=(const float& scalarConstant);
	Vector4& operator=(const Vector2<T>& rhs);
	Vector4& operator=(const Vector3<T>& rhs);

	const T& operator[](size_t idx)const;
	T& operator[](size_t idx);

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
Vector4<T>::Vector4(T s) 
	: Vector4<T>(s, s, s, s)
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
template<typename T>
Vector4<T>::Vector4(const Matrix<T, 1, 4>& matrix) {
	x = matrix.Row(0).value[0];
	y = matrix.Row(0).value[1];
	z = matrix.Row(0).value[2];
	w = matrix.Row(0).value[3];
}


template<typename T> inline
T* Vector4<T>::ConvertToValue() {
	T val[4] = {0, };

	val[0] = x;
	val[1] = y;
	val[2] = z;
	val[3] = w;
	return val;
}

template<typename T> inline
void Vector4<T>::SetXYZW(T _x, T _y, T _z, T _w) {
	x = _x; y = _y; z = _z; w = _w;
}

template<typename T> inline
bool Vector4<T>::IsZero() const {
	return x == T(0) && y == T(0) && z == T(0) && w == T(0);
}

template<typename T> inline
T Vector4<T>::GetLength() const
{
	return sqrt((x * x) + (y * y) + (z * z) + (w * w));
}

template<typename T> inline
void Vector4<T>::Normalize()
{
	T length = GetLength();
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
template<typename T> inline
const T& Vector4<T>::operator[](size_t index) const{
	assert((unsigned)index < (unsigned)4);

	switch (index) {
	case 0: return x;
	case 1: return y;
	case 2: return z;
	case 3: return w;
	}

	return x;
}
template<typename T> inline
T& Vector4<T>::operator[](size_t index) {
	assert((unsigned)index < (unsigned)4);

	switch (index) {
	case 0: return x;
	case 1: return y;
	case 2: return z;
	case 3: return w;
	}

	return x;
}

typedef Vector4<unsigned char> Vec4b;
typedef Vector4<short> Vec4s;
typedef Vector4<unsigned short> Vec4w;
typedef Vector4<int> Vec4i;
typedef Vector4<double> Vec4d;
typedef Vector4<float> Vec4f;


//Global Functions
template<typename T, typename std::enable_if<std::is_floating_point<T>::value>::type* = nullptr>
T DotProduct(const Vector4<T>& v1, const Vector4<T>& v2) {
	return T((v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z));
}
template<typename T>
Vector3<T> Transform(const Matrix<T, 3, 3>& matrix, const Vector3<T>& vec) {
	float x, y, z = 0.0f;

	x = (vec.x * matrix[0]) + (vec.y * matrix[3]) + (vec.z * matrix[6]);
	y = (vec.x * matrix[1]) + (vec.y * matrix[4]) + (vec.z * matrix[7]);
	z = (vec.x * matrix[2]) + (vec.y * matrix[5]) + (vec.z * matrix[8]);

	return Vector3<T>(x, y, z);
}

template<typename T>
Vector4<T> Transform(const Matrix<T, 4, 4>& matrix, const Vector4<T>& vec) {
	float x, y, z, w = 0.0f;

	x = (vec.x * matrix[0]) + (vec.y * matrix[4]) + (vec.z * matrix[8]) + (vec.w * matrix[12]);
	y = (vec.x * matrix[1]) + (vec.y * matrix[5]) + (vec.z * matrix[9]) + (vec.w * matrix[13]);
	z = (vec.x * matrix[2]) + (vec.y * matrix[6]) + (vec.z * matrix[10]) + (vec.w * matrix[14]);
	w = (vec.x * matrix[3]) + (vec.y * matrix[7]) + (vec.z * matrix[11]) + (vec.w * matrix[15]);

	return Vector4<T>(x, y, z, w);
}

template<typename T, typename std::enable_if<std::is_floating_point<T>::value>::type* = nullptr>
Vector4<T> Normalize(const Vector4<T>& v) {
	Vector4<T> ret;
	T length = sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z) + (v.w * v.w));
	ret.x = v.x / length;
	ret.y = v.y / length;
	ret.z = v.z / length;
	ret.w = v.w / length;

	return ret;
}