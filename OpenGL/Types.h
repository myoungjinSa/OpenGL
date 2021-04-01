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
	
	Point2& operator =(const Point2& pt);
	Point2& operator =(Point2&& pt) noexcept;

	T Dot(const Point2& pt) const;
	double DDot(const Point2& pt) const;
	double Cross(const Point2& pt) const;
	bool Inside(const Rect<T>& rect) const;

	T x;
	T y;
};

typedef Point2<int> Point2i;
typedef Point2<long long> Point2l;
typedef Point2<float> Point2f;
typedef Point2<double> Point2d;


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

	double AspectRatio()const;
	bool Empty()const;

	T width;
	T height;
};


typedef Size<int> Size2i;
typedef Size<long long> Size2l;
typedef Size<float> Size2f;
typedef Size<double> Size2d;

template<typename T> class Rect {
public:
	typedef T value_type;

	Rect();
	Rect(T x, T y, T width, T height);
	Rect(const Rect& rect);
	Rect(Rect&& rect) noexcept;
	Rect(const Point2<T>& pt1, const Point2<T>& pt2);

	Rect& operator=(const Rect& rect);
	Rect& operator=(Rect&& rect) noexcept;

	Point2<T> TopLeft() const;
	Point2<T> BottomRight() const;

	Size<T> Size() const;
	T Area() const;
	bool Empty() const;

	bool Contains(const Point2<T>& pt) const;

	T x;
	T y;
	T width;
	T height;

};


typedef Rect<int> Rect2i;
typedef Rect<float> Rect2f;
typedef Rect<double> Rect2d;

////////////////////////2D Point /////////////////////////////////////



