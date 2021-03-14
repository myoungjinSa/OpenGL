#pragma once
#include <initializer_list>
#include <cassert>
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

	explicit Matrix(const T* vals);

	Matrix(std::initializer_list<T> vals);

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

	Matrix<T, n, m> Inverse(int method, bool* p_is_ok = NULL) const;

	Matrix<T, m, n> Multiply(const Matrix<T, m, n>& a) const;

	Matrix<T, m, n> Divide(const Matrix<T, m, n>& a)const;

	const T& operator()(int i, int j) const;
	T& operator()(int i, int j);

	const T& operator() (int i) const;
	T& operator()(int i);

	T val[m * n];
};

/*Utility Method*/
//template<typename _Tp, int m> static double determinant(const Matx<_Tp, m, m>& a);
//template<typename T, int m, int n> static double Normalize(const Matrix<T, m, n>& mat);
//template<typename T, int m, int n> static double Normalize(const Matrix<T, m, n>& mat, int normType);

template<typename T, int m, int n> inline
Matrix<T, m, n>::Matrix()
{
	for (int i = 0; i < channels; i++) val[i] = T(0);
}


template<typename T, int m, int n> inline
Matrix<T, m, n>::Matrix(T v0)
{
	val[0] = v0;
	for (int i = 1; i < channels; i++) val[i] = T(0);
}

template<typename T, int m, int n> inline
Matrix<T, m, n>::Matrix(T v0, T v1)
{
	static_assert((channels >= 2), "Matx should have at least 2 elements.");
	val[0] = v0; val[1] = v1;
	for (int i = 2; i < channels; i++) val[i] = T(0);
}

template<typename T, int m, int n> inline
Matrix<T, m, n>::Matrix(T v0, T v1, T v2)
{
	static_assert(channels >= 3, "Matx should have at least 3 elements.");
	val[0] = v0; val[1] = v1; val[2] = v2;
	for (int i = 3; i < channels; i++) val[i] = T(0);
}
template<typename T, int m, int n> inline
Matrix<T, m, n>::Matrix(T v0, T v1, T v2, T v3)
{
	static_assert(channels >= 4, "Matx should have at least 4 elements.");
	val[0] = v0; val[1] = v1; val[2] = v2; val[3] = v3;
	for (int i = 4; i < channels; i++) val[i] = _Tp(0);
}

template<typename T, int m, int n> inline
Matrix<T, m, n>::Matrix(const T* values)
{
	for (int i = 0; i < channels; i++) val[i] = values[i];
}

template<typename T, int m, int n> inline
Matrix<T, m, n>::Matrix(std::initializer_list<T> list)
{
	assert(list.size() == channels);
	int i = 0;
	for (const auto& elem : list)
	{
		val[i++] = elem;
	}
}


template<typename T, int m, int n> inline
Matrix<T, m, n> Matrix<T, m, n>::All(T alpha) {
	Matrix<T, m, n> mat;
	for (int i = 0; i < m * n; i++)
		mat.val[i] = alpha;
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
		s += val[i] * M.val[i];
	return s;
}

template<typename T, int m, int n> inline
double Matrix<T, m, n>::DDot(const Matrix<T, m, n>& mat) const {
	double s = 0;
	for (int i = 0; i < channels; i++)
		s += (double)val[i] * mat.val[i];

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
	return Matrix<T, 1, n>(&val[i * n]);
}

template<typename T, int m, int n> inline
Matrix<T, m, 1> Matrix<T, m, n>::Col(int j) const {
	assert((unsigned)j < (unsigned)n);
	Matrix<T, m, 1> v;
	for (int i = 0; i < m; i++)
		v.val[i] = val[i * n + j];
	return v;
}

template<typename T, int m, int n> inline
typename Matrix<T, m, n>::diag_type Matrix<T, m, n>::Diagonal() const
{
	diag_type d;
	for (int i = 0; i < shortdim; i++)
		d.val[i] = val[i * n + i];
	return d;
}
template<typename _Tp, int m, int n> inline
Matrix<_Tp, n, m> Matrix<_Tp, m, n>::Transpose() const
{
	return Matrix<_Tp, n, m>(*this);
}
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

template<typename _Tp, int m, int n> inline
const _Tp& Matrix<_Tp, m, n>::operator()(int i, int j) const
{
	assert((unsigned)i < (unsigned)m && (unsigned)j < (unsigned)n);
	return this->val[i * n + j];
}

template<typename _Tp, int m, int n> inline
_Tp& Matrix<_Tp, m, n>::operator ()(int i, int j)
{
	assert((unsigned)i < (unsigned)m && (unsigned)j < (unsigned)n);
	return val[i * n + j];
}

template<typename _Tp, int m, int n> inline
const _Tp& Matrix<_Tp, m, n>::operator ()(int i) const
{
	static_assert(m == 1 || n == 1, "Single index indexation requires matrix to be a column or a row");
	assert((unsigned)i < (unsigned)(m + n - 1));
	return val[i];
}

template<typename _Tp, int m, int n> inline
_Tp& Matrix<_Tp, m, n>::operator ()(int i)
{
	static_assert(m == 1 || n == 1, "Single index indexation requires matrix to be a column or a row");
	assert((unsigned)i < (unsigned)(m + n - 1));
	return val[i];
}


////////////////////////////////////////////////////////////////////
//Vector


template<typename T, int count>
class Vector  : public Matrix<T, count, 1>
{
	typedef T value_type;
	enum {
		channels = count,
		_dummy_enum_finalizer = 0
	};

	Vector();

	Vector(T v0);
	Vector(T v0, T v1);
	Vector(T v0, T v1, T v2);
	Vector(T v0, T v1, T v2, T v3);

	explicit Vector(const T* values);

	Vector(std::initializer_list<T> list);
	Vector(const Vector<T, count>& v);

	static Vector All(T alpha);

	Vector Multiply(const Vector<T, count>& v)const;

	template<typename T2> operator Vector<T2, count>() const;

	//conjugation (makes sense for complex numbers and quaternions)
	Vector Conjugation() const;
	Vector Cross(const Vector& v)const;

	const T& operator[](int idx) const;
	T& operator[](int idx);

	const T& operator()(int idx) const;
	T& operator()(int idx);


};
typedef Vector<unsigned char, 2> Vec2b;
typedef Vector<unsigned char, 3> Vec3b;
typedef Vector<unsigned char, 4> Vec4b;

typedef Vector<short, 2> Vec2s;
typedef Vector<short, 3> Vec3s;
typedef Vector<short, 4> Vec4s;

typedef Vector<unsigned short, 2> Vec2w;
typedef Vector<unsigned short, 3> Vec3w;
typedef Vector<unsigned short, 4> Vec4w;

typedef Vector<int, 2> Vec2i;
typedef Vector<int, 3> Vec3i;
typedef Vector<int, 4> Vec4i;

typedef Vector<float, 2> Vec2f;
typedef Vector<float, 3> Vec3f;
typedef Vector<float, 4> Vec4f;

typedef Vector<double, 2> Vec2d;
typedef Vector<double, 3> Vec3d;
typedef Vector<double, 4> Vec4d;


template<typename T, int count> inline
Vector<T, count>::Vector() {

}

template<typename T, int count> inline
Vector<T, count>::Vector(T v0) 
	: Matrix<T, count, 1>(v0) {

}

template<typename T, int count> inline
Vector<T, count>::Vector(T v0, T v1) 
	: Matrix<T, count, 1>(v0, v1){

}

template<typename T, int count> inline
Vector<T, count>::Vector(T v0, T v1, T v2)
	: Matrix<T, count, 1>(v0, v1, v2) {

}
template<typename T, int count> inline
Vector<T, count>::Vector(T v0, T v1, T v2, T v3)
	: Matrix<T, count, 1>(v0, v1, v2, v3) {}


template<typename T, int count> inline
Vector<T, count>::Vector(const T* values) 
	:Matrix<T, count, 1>(values){

}

template<typename T, int count> inline
Vector<T, count>::Vector(std::initializer_list<T> list)
	: Matrix<T, count, 1>(list) {}

template<typename T, int count> inline
Vector<T, count>::Vector(const Vector<T, count>& m)
	: Matrix<T, count, 1>(m.val) {}


template<typename T, int count> inline
Vector<T, count> Vector<T, count>::All(T alpha)
{
	Vector v;
	for (int i = 0; i < cn; i++) v.val[i] = alpha;
	return v;
}

template<typename T, int count> inline
Vector<T, count> Vector<T, count>::Multiply(const Vector<T, count>& v) const
{
	Vector<T, count> w;
	for (int i = 0; i < count; i++) w.val[i] = this->val[i] * v.val[i];
	return w;
}

template<typename T> Vector<T, 2> inline Conjugate(const Vector<T, 2>& v) {
	return Vector<T, 2>(v[0], -v[1]);
}

template<typename T> Vector<T, 4> inline Conjugate(const Vector<T, 4>& v)
{
	return Vector<T, 4>(v[0], -v[1], -v[2], -v[3]);
}
template<> inline
Vector<float, 2> Vector<float, 2>::Conjugation() const
{
	return Conjugate(*this);
}
template<> inline
Vector<double, 2> Vector<double, 2>::Conjugation() const
{
	return Conjugate(*this);
}

template<> inline
Vector<float, 4> Vector<float, 4>::Conjugation() const
{
	return Conjugate(*this);
}

template<> inline
Vector<double, 4> Vector<double, 4>::Conjugation() const
{
	return Conjugate(*this);
}

template<typename T, int count> inline
Vector<T, count> Vector<T, count>::Cross(const Vector<T, count>&) const
{
	static_assert(cn == 3, "for arbitrary-size vector there is no cross-product defined");
	return Vector<T, count>();
}


template<> inline
Vector<float, 3> Vector<float, 3>::Cross(const Vector<float, 3>& v) const
{
	return Vector<float, 3>(this->val[1] * v.val[2] - this->val[2] * v.val[1],
		this->val[2] * v.val[0] - this->val[0] * v.val[2],
		this->val[0] * v.val[1] - this->val[1] * v.val[0]);
}

template<> inline
Vector<double, 3> Vector<double, 3>::Cross(const Vector<double, 3>& v) const
{
	return Vector<double, 3>(this->val[1] * v.val[2] - this->val[2] * v.val[1],
		this->val[2] * v.val[0] - this->val[0] * v.val[2],
		this->val[0] * v.val[1] - this->val[1] * v.val[0]);
}

template<typename T, int count> template<typename T2> inline
Vector<T, count>::operator Vector<T2, count>() const
{
	Vector<T2, count> v;
	for (int i = 0; i < count; i++) v.val[i] = this->val[i];
	return v;
}


template<typename T, int count> inline
const T& Vector<T, count>::operator [](int i) const
{
	assert((unsigned)i < (unsigned)count);
	return this->val[i];
}

template<typename T, int count> inline
T& Vector<T, count>::operator [](int i)
{
	assert((unsigned)i < (unsigned)count);
	return this->val[i];
}

template<typename T, int count> inline
const T& Vector<T, count>::operator()(int i) const
{
	assert((unsigned)i < (unsigned)count);
	return this->val[i];
}

template<typename T, int count> inline
T& Vector<T, count>::operator ()(int i)
{
	assert((unsigned)i < (unsigned)count);
	return this->val[i];
}