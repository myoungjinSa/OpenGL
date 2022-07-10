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

template<typename T> class Volume {
public:
	Vector3<T> min;
	Vector3<T> max;

	Volume() {}
	Volume(const Vector3<T>& _min, const Vector3<T>& _max) {
		min = _min;
		max = _max;
	}

	T GetXSize() const { return max.x - min.x; }
	T GetYSize() const { return max.y - min.y; }
	T GetZSize() const { return max.z - min.z; }

	Vector3<T> GetSize() const { return Vector3<T>(max.x - min.x, max.y - min.y, max.z - min.z); }
	Vector3<T> GetCenter() const { return Vector3<T>((max.x + min.x) / 2.0f, (max.y + min.y) / 2.0f, (max.z + min.z) / 2.0f); }
	void SetCenter(const Vector3<T>& center);
	void SetZero();

	bool IsZero() const { return (min.IsZero() && max.IsZero()); }
	bool IsEmpty() const;

	void Inflate(const Vector3<T>& delta) { min -= delta; max += delta; }
	void Move(const Vector3<T>& offset) { min += offset; max += offset; }
	void Normalize();

	operator Rect<T>() const { return Rect<T>(min.x, min.y, max.x, max.y); }

	Volume operator*(T factor) const { return Volume(min * factor, max * factor); }
	Volume operator*(Vector3<T> vol) const { return Volume(min * vol, max * vol); }
	Volume operator+(Vector3<T> vol) const { return Volume(min + vol, max + vol); }
	Volume operator-(Vector3<T> vol) const { return Volume(min - vol, max - vol); }

	void operator|=(const Vector3<T>& vector);
	void operator|=(const Volume& other);
};
typedef Volume<int>    Volumei;
typedef Volume<float>  Volumef;
typedef Volume<double> Volumed;

template<typename T> class Line {
public:
	Vector3<T> point;
	Vector3<T> direction;

	Line(){}
	Line(const Vector3<T>& _point, const Vector3<T>& _direction) { point = _point; direction = _direction; }
};

typedef Line<int>    Linei;
typedef Line<float>  Linef;
typedef Line<double> Lined;
///////////////////////////////////////////////////////////////////////////////////

template<typename T> class Plane {
public:
	Vector3<T> normal;
	T          distance;

	Plane(){}
	Plane(const Vector3<T>& _normal, T _distance) { normal = _normal; distance = _distance; }

	bool operator==(const Plane& other) const { return normal == other.normal && distance == other.distance; }

	void Build(const Vec3f& v1, const Vec3f& v2, const Vec3f& v3) {
		Vec3f edge1 = v1 - v2;
		Vec3f edge2 = v3 - v2;
		normal = edge1.Cross(edge2);

		normal.Normalize();
		distance = -normal.DotProduct(v1);
	}

	void Build(const Vec4f& _v1, const Vec4f& _v2, const Vec4f& _v3) {
		Vec3f v1 = Vec3f(_v1.x, _v1.y, _v1.z);
		Vec3f v2 = Vec3f(_v2.x, _v2.y, _v2.z);
		Vec3f v3 = Vec3f(_v3.x, _v3.y, _v3.z);

		Vec3f edge1 = v1 - v2;
		Vec3f edge2 = v3 - v2;
		normal = edge1.Cross(edge2);
	
		normal.Normalize();
		distance = -normal.DotProduct(v1);
	}

	void Set(const Vec3f &_normal, T _distance) {
		normal = _normal;
		distance = _distance;
	}

	void Set(const Vec4f& plane) {
		normal.Set(plane.x, plane.y, plane.z);
		distance = plane.w;
	}

	T GetDistance(const Vec3f& point) const {
		return normal.DotProduct(point) + distance;
	}

	Vector3<T> GetIntersection(const Line<T>& line) const{
		T t = (-distance - line.point.DotProduct(normal)) / line.direction.DotProduct(normal);
		return line.point + line.direction * t;
	}

	bool IsIntersected(const Line<T>& line) const{
		if (line.direction.DotProduct(normal))
			return true;

		return false;
	}

	bool Normalize() {
		T length = normal.GetLength();
		if (length == 0)
			return false;

		normal /= length;
		distance /= length;
		return true;
	}

	Plane operator-() const { return Plane(-normal, -distance); }
};

typedef Plane<int>    Planei;
typedef Plane<float>  Planef;
typedef Plane<double> Planed;