#pragma once
#include "Matrix.h"
#include <numeric>

//템플릿 전방선언
template<typename> class Size;
template<typename> class Rect;
template<typename> class Point2;
template<typename> class Point3;

template<typename T> class Point2
{
public:
	typedef T value_type;

	Point2();
	Point2(T x, T y);
	Point2(const Point2& pt);
	Point2(Point2&& pt) noexcept;
	Point2(const Size<T>& size);
	//Point2(const Vector2<T>& vec);
	
	Point2& operator=(const Point2& pt);
	Point2& operator=(Point2&& pt) noexcept;

	T DotProduct(const Point2& pt) const;
	double DDotProduct(const Point2& pt) const;
	double Cross(const Point2& pt) const;
	bool Inside(const Rect<T>& rect) const;

	Point2<T> operator+(const Point2<T>& other);
	Point2<T> operator-(const Point2<T>& other);
	Point2<T> operator*(const Point2<T>& other);
	Point2<T> operator/(const Point2<T>& other);

	T x;
	T y;
};

typedef Point2<int> Point2i;
typedef Point2<long long> Point2l;
typedef Point2<float> Point2f;
typedef Point2<double> Point2d;

template<typename T> class Point3
{
public:
	typedef T value_type;

	Point3();
	Point3(T x, T y, T z);
	Point3(const Point3<T>& pt);
	Point3(Point3<T>&& pt) noexcept;
	
	Point3& operator=(const Point3<T>& pt);
	Point3& operator=(Point3<T>&& pt) noexcept;

	T DotProduct(const Point3<T>& pt) const;
	double DDotProduct(const Point3<T>& pt) const;
	double Cross(const Point3<T>& pt) const;

	T x;
	T y;
	T z;
};

typedef Point3<int> Point3i;
typedef Point3<long long> Point3l;
typedef Point3<float> Point3f;
typedef Point3<double> Point3d;

template<typename T> class Size
{
public:
	typedef T value_type;

	Size();
	Size(T width, T height);
	Size(const Size& size);
	Size(Size&& size)noexcept;
	Size(const Point2<T>& pt);

	Size& operator=(const Size& size);
	Size& operator=(Size&& size) noexcept;

	T Area() const;
	void Set(T _width, T _height);
	double AspectRatio()const;
	
	bool Empty()const;

	T width;
	T height;
};


typedef Size<int> Size2i;
typedef Size<long long> Size2l;
typedef Size<float> Size2f;
typedef Size<double> Size2d;
typedef Size<unsigned int> Size2u;

template<typename T> class Rect {
public:
	typedef T value_type;

	Rect();
	Rect(T _left, T _top, T _right, T _bottom);
	Rect(const Rect& rect);
	Rect(Rect&& rect) noexcept;
	Rect(const Point2<T>& pt1, const Point2<T>& pt2);

	Rect& operator=(const Rect& rect);
	Rect& operator=(Rect&& rect) noexcept;

	Point2<T> TopLeft() const;
	Point2<T> BottomRight() const;

	Size<T> GetSize() const;
	T Area() const;
	bool Empty() const;

	bool Contains(const Point2<T>& pt) const;

	T GetWidth() const;
	T GetHeight() const;
	
	T left;
	T top;
	T right;
	T bottom;

};


typedef Rect<int> Rect2i;
typedef Rect<float> Rect2f;
typedef Rect<double> Rect2d;

////////////////////////2D Point /////////////////////////////////////



