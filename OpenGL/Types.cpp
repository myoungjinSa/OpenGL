#include "Types.h"
#include <algorithm>

template<typename T> 
Point2<T>::Point2()
	: x(0), y(0)
{}

template<typename T> inline
Point2<T>::Point2(T _x, T _y)
	: x(_x), y(_y)
{}

template<typename T> inline
Point2<T>::Point2(const Point2& pt)
	: x(pt.x), y(pt.y)
{}

template<typename T> inline
Point2<T>::Point2(Point2&& pt) noexcept
	:x(std::move(pt.x)), y(std::move(pt.y))
{}

template<typename T> inline
Point2<T>::Point2(const Size<T>& size)
	: x(size.width), y(size.height)
{}

//template<typename T> inline
//Point2<T>::Point2(const Vector2<T>& v)
//	: x(v.x), y(v.y)
//{}

template<typename T> inline
Point2<T>& Point2<T>::operator=(const Point2& pt) {
	x = pt.x;
	y = pt.y;
	return *this;
}

template<typename T> inline
Point2<T>& Point2<T>::operator=(Point2&& pt) noexcept {
	x = std::move(pt.x);
	y = std::move(pt.y);
	return *this;
}

template<typename T> inline
T Point2<T>::Dot(const Point2& pt) const {
	return x * pt.x + y * pt.y;			//need to saturate cast
}


template<typename T> inline
double Point2<T>::DDot(const Point2& pt) const {
	return (double)x * pt.x + (double)y * pt.y;
}

template<typename T> inline
double Point2<T>::Cross(const Point2& pt) const {
	return (double)x * pt.y - (double)y * pt.x;
}

template<typename T> inline
bool Point2<T>::Inside(const Rect<T>& rect) const {
	return rect.Contains(*this);
}


template<typename T> static inline
Point2<T>& operator+=(Point2<T>& left, const Point2<T>& right) {
	left.x += right.x;
	left.y += right.y;
	return left;
}

template<typename T> static inline
Point2<T>& operator-=(Point2<T>& left, const Point2<T>& right) {
	left.x -= right.x;
	left.y -= right.y;
	return left;
}

template<typename T> static inline
Point2<T>& operator *=(Point2<T>& left, int right) {
	left.x = left.x * right;			//need to saturate cast
	left.y = left.y * right;
	return left;
}

template<typename T> static inline
Point2<T>& operator *=(Point2<T>& left, float right) {
	left.x = left.x * right;			//need to saturate cast
	left.x = left.x * right;
	return left;
}

template<typename T> static inline
Point2<T>& operator *=(Point2<T>& left, double right) {
	left.x = left.x * right;			//need to saturate cast
	left.y = left.y * right;
	return left;
}

template<typename T> static inline
Point2<T>& operator /=(Point2<T>& left, int right) {
	assert(right != 0);

	left.x = left.x / right;			//need to saturate cast
	left.y = left.x / right;
	return left;
}

template<typename T> static inline
Point2<T>& operator /=(Point2<T>& left, float right) {
	assert(right != 0.0f);

	left.x = left.x / right;
	left.y = left.y / right;
	return left;
}

template<typename T> static inline
Point2<T>& operator /(Point2<T>& left, double right) {
	assert(right != 0.0);

	left.x = left.x / right;
	left.y = left.y / right;
	return left;
}


//명시적 템플릿 인스턴스화 선언
template class Point2<int>;
template class Point2<long long>;
template class Point2<float>;
template class Point2<double>;


///////////////////////////////Size////////////////////////////////////
template<typename T> inline
Size<T>::Size()
	:width(0), height(0)
{}

template<typename T> inline
Size<T>::Size(T _width, T _height)
	: width(_width), height(_height)
{}

template<typename T> inline
Size<T>::Size(const Size& sz)
	: width(sz.width), height(sz.height)
{}

template<typename T> inline
Size<T>::Size(Size&& sz) noexcept
	:width(std::move(sz.width)), height(std::move(sz.height))
{

}

template<typename T> inline
Size<T>::Size(const Point2<T>& pt)
	: width(pt.x), height(pt.y)
{}

template<typename T>inline
Size<T>& Size<T>::operator=(const Size<T>& size) {
	width = size.width; height = size.height;
	return *this;
}

template<typename T> inline
Size<T>& Size<T>::operator=(Size<T>&& size) noexcept {
	width = std::move(size.width); height = std::move(size.height);
	return *this;
}

template<typename T> inline
T Size<T>::Area() const {
	const T result = width * height;
	//assert(!std::numeric_limits<T>::is_integer || width == 0 || result / width == height);
	return result;
}

template<typename T> inline
double Size<T>::AspectRatio() const {
	return width / static_cast<double>(height);
}

template<typename T>inline
bool Size<T>::Empty() const {
	return width <= 0 || height <= 0;
}

template<typename T> static inline
Size<T>& operator *=(Size<T>& left, T right) {
	left.width *= right;
	left.height *= right;
	return left;
}

template<typename T> static inline
Size<T>& operator *(const Size<T>& left, T right) {
	Size<T> tmp(left);
	tmp *= right;
	return tmp;
}

template<typename T> static inline
Size<T>& operator/(const Size<T>& left, T right) {
	Size<T> tmp(left);
	tmp /= right;
	return tmp;
}

template<typename T> static inline
Size<T>& operator+=(Size<T>& left, const Size<T>& right) {
	left.width += right.width;
	left.height += right.height;
	return left;
}

template<typename T> static inline
Size<T> operator+(const Size<T>& left, const Size<T>& right) {
	Size<T> tmp(left);
	tmp += right;
	return tmp;
}

template<typename T> static inline
Size<T> operator-=(const Size<T>& left, const Size<T>& right) {
	left.width -= right.width;
	right.height -= right.height;
	return left;
}

template<typename T> static inline
Size<T> operator -(const Size<T>& left, const Size<T>& right) {
	Size<T> tmp(left);
	tmp -= right;
	return tmp;
}

template<typename T> static inline
bool operator ==(const Size<T>& left, const Size<T>& right) {
	return left.width == right.width && left.height == right.height;
}


template<typename T> static inline
bool operator !=(const Size<T>& left, const Size<T>& right) {
	return !(left == right);
}

template class Size<int>;
template class Size<long long>;
template class Size<float>;
template class Size<double>;
template class Size<unsigned int>;


///////////////////////Rect //////////////////////////////////

template<typename T> inline
Rect<T>::Rect() 
	:x(0), y(0), width(0), height(0)
{

}
template<typename T> inline
Rect<T>::Rect(T _x, T _y, T _width, T _height)
	:x(_x), y(_y), width(_width), height(_height)
{}

template<typename T> inline
Rect<T>::Rect(const Rect<T>& rect) 
	:x(rect.x), y(rect.y), width(rect.width), height(rect.height)
{}

template<typename T> inline
Rect<T>::Rect(Rect<T>&& rect) noexcept 
	:x(std::move(rect.x)), y(std::move(rect.y)), width(std::move(rect.width)), height(std::move(rect.height))
{}


template<typename T> inline
Rect<T>::Rect(const Point2<T>& pt1, const Point2<T>& pt2) {
	x = std::min(pt1.x, pt2.x);
	y = std::min(pt1.y, pt2.y);
	width = std::max(pt1.x, pt2.x) - x;
	height = std::max(pt1.y, pt2.y) - y;
}

template<typename T> inline
Rect<T>& Rect<T>::operator=(const Rect<T>& rect) {
	x = rect.x;
	y = rect.y;
	width = rect.width;
	height = rect.height;
	return *this;
}

template<typename T> inline
Rect<T>& Rect<T>::operator=(Rect<T>&& rect) noexcept {
	x = std::move(rect.x);
	y = std::move(rect.y);
	width = std::move(rect.width);
	height = std::move(rect.height);
	return *this;
}

template<typename T> inline
Point2<T> Rect<T>::TopLeft() const {
	return Point2<T>(x, y);
}

template<typename T> inline
Point2<T> Rect<T>::BottomRight() const {
	return Point2<T>(x + width, y + height);
}

template<typename T> inline
Size<T> Rect<T>::Size() const {
	return Size<T>(width, height);
}

template<typename T> inline
T Rect<T>::Area() const {
	const T result = width * height;
	assert(!std::numeric_limits<T>::is_integer || width == 0 || result / width == height);
	return result;
}

template <typename T> inline
bool Rect<T>::Empty() const {
	return width <= 0 || height <= 0;
}
template<typename T> inline
bool Rect<T>::Contains(const Point2<T>& pt) const {
	return x <= pt.x && pt.x < x + width && y <= pt.y && pt.y < y + height;
}

template<typename T> static inline
Rect<T>& operator += (Rect<T>& left, const Point2<T>& right) {
	left.x += right.x;
	left.y += right.y;
	return left;
}

template<typename T> static inline
Rect<T>& operator -=(Rect<T>& a, const Point2<T>& b) {
	a.x -= b.x;
	a.y -= b.y;
	return a;
}

template<typename T> static inline
Rect<T>& operator +=(Rect<T>& a, const Size<T>& b) {
	a.width += b.width;
	a.height += b.height;
	return a;
}


template<typename T> static inline
Rect<T>& operator -=(Rect<T>& a, const Size<T>& b) {
	a.width -= b.width;
	a.height -= b.height;
	return a;
}

template<typename T> static inline
Rect<T>& operator &=(Rect<T>& a, const Rect<T>& b) {
	T x1 = std::max(a.x, b.x);
	T y1 = std::max(a.y, b.y);

	a.width = std::min(a.x + a.width, b.x + b.width) - x1;
	a.height = std::min(a.y + a.height, b.x + b.height) - y1;
	a.x = x1;
	a.y = y1;
	if (a.width <= 0 || a.height <= 0)
		a = Rect<T>();
	return a;
}

template<typename T> static inline
Rect<T>& operator |=(Rect<T>& a, const Rect<T>& b) {
	if (a.Empty()) {
		a = b;
	}
	else if (!b.Empty()) {
		T x1 = std::min(a.x, b.y);
		T y1 = std::min(a.y, b.y);
		a.width = std::max(a.x + a.width, b.x + b.width) - x1;
		a.height = std::max(a.y + a.height, b.y + b.height) - y1;
		a.x = x1;
		a.y = y1;
	}
	return a;
}

template<typename T> static inline
bool operator==(const Rect<T>& a, const Rect<T>& b) {
	return a.x == b.x && a.y == b.y && a.width == b.width && a.height == b.height;
}

template<typename T> static inline
bool operator !=(const Rect<T>& a, const Rect<T>& b) {
	return a.x != b.x || a.y != b.y || a.width != b.width || a.height != b.height;
}

template<typename T> static inline
Rect<T> operator+(const Rect<T>& a, const Point2<T>& b) {
	return Rect<T>(a.x + b.x, a.y + b.y, a.width, a.height);
}

template<typename T> static inline
Rect<T> operator-(const Rect<T>& a, const Point2<T>& b) {
	return Rect<T>(a.x - b.x, a.y - b.y, a.width, a.width);
}

template<typename T> static inline
Rect<T> operator + (const Rect<T>& a, const Size<T>& b)
{
	return Rect<T>(a.x, a.y, a.width + b.width, a.height + b.height);
}

template<typename T> static inline
Rect<T> operator & (const Rect<T>& a, const Rect<T>& b)
{
	Rect<T> c = a;
	return c &= b;
}

template<typename T> static inline
Rect<T> operator | (const Rect<T>& a, const Rect<T>& b)
{
	Rect<T> c = a;
	return c |= b;
}

