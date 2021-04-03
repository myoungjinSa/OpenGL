#pragma once
#include <initializer_list>
#include <cassert>
#include <cmath>

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


};

template<typename T>
class Vector3{
public:
	typedef T value_type;

	Vector3();
	Vector3(T v0, T v1, T v2);
	//explicit Vector3(const T* values);
	Vector3(const Vector3<T>& v);

	Vector3 Multiply(const Vector3<T>& v);
	template<typename T2> operator Vector3<T2>() const;

	Vector3 Cross(const Vector3& v) const;

	void SetXYZ(T x, T y, T z);
	Vector3<T> GetXYZ() const;

	
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
Vector3<T>::Vector3(const Vector3<T>& v) 
	: x(v.x), y(v.y), z(v.z)
{

}

template<typename T> inline
Vector3<T> Vector3<T>::Multiply(const Vector3<T>& v) {
	Vector3<T> ret;
	ret.x = x * v.x;
	ret.y = y * v.y;
	ret.z = z * v.z;
	return ret;
}


template<> inline
Vector3<float> Vector3<float>::Cross(const Vector3<float>& v) const {
	return Vector3<float>(y * z - z * y, z * x - x * z, x * y - y * x);
}
template<typename T> template<typename T2> inline
Vector3<T>::operator Vector3<T2>() const
{
	Vector3<T2> v;
	v.x = x; v.y = y; v.z = z;
	return v;
}

template<typename T> inline
void Vector3<T>::SetXYZ(T _x, T _y, T _z) {
	x = _x; y = _y; z = _z;
}



template<typename T> inline
Vector3<T> Vector3<T>::GetXYZ() const{
	Vector3<T> v(x, y, z);
	return v;
}
typedef Vector3<unsigned char> Vec3b;
typedef Vector3<short> Vec3s;
typedef Vector3<unsigned short> Vec3w;
typedef Vector3<int> Vec3i;
typedef Vector3<double> Vec3d;
typedef Vector3<float> Vec3f;

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
//Vector3<float> Normalize(const Vector3<float>& from, const Vector3<float>& to) {
//	Vector3<float> ret;
//
//	ret = from - to;
//
//	float length = sqrtf((ret.x * ret.x) + (ret.y * ret.y) + (ret.z * ret.z));
//
//	ret.x = ret.x / length;
//	ret.y = ret.y / length;
//	ret.z = ret.z / length;
//
//	return ret;
//}

//Vector3<double> Normalize(const Vector3<double>& from, const Vector3<double>& to) {
//	Vector3<double> ret;
//
//	ret = from - to;
//
//	double length = sqrt((ret.x * ret.x) + (ret.y * ret.y) + (ret.z * ret.z));
//
//	ret.x = ret.x / length;
//	ret.y = ret.y / length;
//	ret.z = ret.z / length;
//
//	return ret;
//}

////////////////////////////////////////////////////////////////////
//Vector

//
//template<typename T, int count> 
//class Vector  : public Matrix<T, count, 1>
//{
//public:
//	typedef T value_type;
//
//	enum {
//		channels = count,
//		_dummy_enum_finalizer = 0
//	};
//
//	
//	Vector();
//
//	Vector(T v0);
//	Vector(T v0, T v1);
//	Vector(T v0, T v1, T v2);
//	Vector(T v0, T v1, T v2, T v3);
//
//	explicit Vector(const T* valueues);
//
//	Vector(std::initializer_list<T> list);
//	Vector(const Vector<T, count>& v);
//
//	static Vector All(T alpha);
//
//	Vector Multiply(const Vector<T, count>& v)const;
//
//	template<typename T2> operator Vector<T2, count>() const;
//
//	//conjugation (makes sense for complex numbers and quaternions)
//	Vector Conjugation() const;
//	Vector Cross(const Vector& v)const;
//
//	const T& operator[](int idx) const;
//	T& operator[](int idx);
//
//	const T& operator()(int idx) const;
//	T& operator()(int idx);
//
//
//	void SetValues(const Vector<T, count>& vec);
//	
//	//T& p = Matrix<T, count, 1>::value;
//	//static Vector Bind(T x, T y, T z, T w);
//};
//
//
//typedef Vector<unsigned char, 2> Vec2b;
//typedef Vector<unsigned char, 3> Vec3b;
//typedef Vector<unsigned char, 4> Vec4b;
//
//typedef Vector<short, 2> Vec2s;
//typedef Vector<short, 3> Vec3s;
//typedef Vector<short, 4> Vec4s;
//
//typedef Vector<unsigned short, 2> Vec2w;
//typedef Vector<unsigned short, 3> Vec3w;
//typedef Vector<unsigned short, 4> Vec4w;
//
//typedef Vector<int, 2> Vec2i;
//typedef Vector<int, 3> Vec3i;
//typedef Vector<int, 4> Vec4i;
//
//typedef Vector<float, 2> Vec2f;
//typedef Vector<float, 3> Vec3f;
//typedef Vector<float, 4> Vec4f;
//
//typedef Vector<double, 2> Vec2d;
//typedef Vector<double, 3> Vec3d;
//typedef Vector<double, 4> Vec4d;
//
//
//template<typename T, int count> inline
//Vector<T, count>::Vector() {
//
//}
//
//template<typename T, int count> inline
//Vector<T, count>::Vector(T v0) 
//	: Matrix<T, count, 1>(v0) {
//
//}
//
//template<typename T, int count> inline
//Vector<T, count>::Vector(T v0, T v1) 
//	: Matrix<T, count, 1>(v0, v1){
//
//}
//
//template<typename T, int count> inline
//Vector<T, count>::Vector(T v0, T v1, T v2)
//	: Matrix<T, count, 1>(v0, v1, v2) {
//
//}
//template<typename T, int count> inline
//Vector<T, count>::Vector(T v0, T v1, T v2, T v3)
//	: Matrix<T, count, 1>(v0, v1, v2, v3) {}
//
//
//template<typename T, int count> inline
//Vector<T, count>::Vector(const T* valueues) 
//	:Matrix<T, count, 1>(valueues){
//
//}
//
//template<typename T, int count> inline
//Vector<T, count>::Vector(std::initializer_list<T> list)
//	: Matrix<T, count, 1>(list) {}
//
//template<typename T, int count> inline
//Vector<T, count>::Vector(const Vector<T, count>& m)
//	: Matrix<T, count, 1>(m.value) {}
//
//
//template<typename T, int count> inline
//Vector<T, count> Vector<T, count>::All(T alpha)
//{
//	Vector v;
//	for (int i = 0; i < count; i++) v.value[i] = alpha;
//	return v;
//}
//
//template<typename T, int count> inline
//Vector<T, count> Vector<T, count>::Multiply(const Vector<T, count>& v) const
//{
//	Vector<T, count> w;
//	for (int i = 0; i < count; i++) w.value[i] = this->value[i] * v.value[i];
//	return w;
//}
//
//template<typename T> Vector<T, 2> inline Conjugate(const Vector<T, 2>& v) {
//	return Vector<T, 2>(v[0], -v[1]);
//}
//
//template<typename T> Vector<T, 4> inline Conjugate(const Vector<T, 4>& v)
//{
//	return Vector<T, 4>(v[0], -v[1], -v[2], -v[3]);
//}
//template<> inline
//Vector<float, 2> Vector<float, 2>::Conjugation() const
//{
//	return Conjugate(*this);
//}
//template<> inline
//Vector<double, 2> Vector<double, 2>::Conjugation() const
//{
//	return Conjugate(*this);
//}
//
//template<> inline
//Vector<float, 4> Vector<float, 4>::Conjugation() const
//{
//	return Conjugate(*this);
//}
//
//template<> inline
//Vector<double, 4> Vector<double, 4>::Conjugation() const
//{
//	return Conjugate(*this);
//}
//
//template<typename T, int count> inline
//Vector<T, count> Vector<T, count>::Cross(const Vector<T, count>&) const
//{
//	static_assert(count == 3, "for arbitrary-size vector there is no cross-product defined");
//	return Vector<T, count>();
//}
//
//
//template<> inline
//Vector<float, 3> Vector<float, 3>::Cross(const Vector<float, 3>& v) const
//{
//	return Vector<float, 3>(this->value[1] * v.value[2] - this->value[2] * v.value[1],
//		this->value[2] * v.value[0] - this->value[0] * v.value[2],
//		this->value[0] * v.value[1] - this->value[1] * v.value[0]);
//}
//
//template<> inline
//Vector<double, 3> Vector<double, 3>::Cross(const Vector<double, 3>& v) const
//{
//	return Vector<double, 3>(this->value[1] * v.value[2] - this->value[2] * v.value[1],
//		this->value[2] * v.value[0] - this->value[0] * v.value[2],
//		this->value[0] * v.value[1] - this->value[1] * v.value[0]);
//}
//
//template<typename T, int count> template<typename T2> inline
//Vector<T, count>::operator Vector<T2, count>() const
//{
//	Vector<T2, count> v;
//	for (int i = 0; i < count; i++) v.value[i] = this->value[i];
//	return v;
//}
//
//
//template<typename T, int count> inline
//const T& Vector<T, count>::operator [](int i) const
//{
//	assert((unsigned)i < (unsigned)count);
//	return this->value[i];
//}
//
//template<typename T, int count> inline
//T& Vector<T, count>::operator [](int i)
//{
//	assert((unsigned)i < (unsigned)count);
//	return this->value[i];
//}
//
//template<typename T, int count> inline
//const T& Vector<T, count>::operator()(int i) const
//{
//	assert((unsigned)i < (unsigned)count);
//	return this->value[i];
//}
//
//template<typename T, int count> inline
//T& Vector<T, count>::operator ()(int i)
//{
//	assert((unsigned)i < (unsigned)count);
//	return this->value[i];
//}
//
//template<typename T, int count> inline
//void Vector<T, count>::SetValues(const Vector<T, count>& vec) {
//	for (size_t iVal = 0; iVal < count; iVal++) {
//		this->value[iVal] = vec.value[iVal];
//	}
//}
//


