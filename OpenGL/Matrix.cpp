#include "Matrix.h"

double GetDeterminant(const Matrix<float, 2, 2>& other){
	return (double)(other.value[0] * other.value[3] - other.value[2] * other.value[3]);
}

double GetDeterminant(const Matrix<float, 3, 3>& other){
/*
	*	a	b	c
	*	d	e	f
	*	g	h	i
	*
	*	a	b	c	a	b	c
	*	d	e	f	d	e	f
	*	g	h	i	g	h	i
	*
	*	det(A) = aei + bfg + cdh - afh - bdi - ceg
	*
	*/

	float a = other.value[0];
	float b = other.value[1];
	float c = other.value[2];

	float d = other.value[3];
	float e = other.value[4];
	float f = other.value[5];

	float g = other.value[6];
	float h = other.value[7];
	float i = other.value[8];

	double det = ((a * e * i) + (b * f * g) + (c * d * h));
	det = det - a * f * h;
	det = det - b * d * i;
	det = det - c * e * g;

	return (double)det;
}



template<typename T>
Vector2<T> Vector2<T>::GetMidPoint(const Vector2<T>& start, const Vector2<T>& end) {
	Vector2<T> midPoint;
	midPoint.x = (start.x + end.x) / 2.0f;
	midPoint.y = (start.y + end.y) / 2.0f;
	return midPoint;
}

////////////////////////////
//Vector3
template<typename T> inline
Vector3<T> Vector3<T>::GetMidPoint(const Vector3<T>& start, const Vector3<T>& end) {
	Vector3<T> midPoint;
	midPoint.x = (start.x + end.x) / 2.0f;
	midPoint.y = (start.y + end.y) / 2.0f;
	midPoint.z = (start.z + end.z) / 2.0f;
	return midPoint;
}

//-----------------------------------------------------------------------------------
template<typename T>
Vector4<T> Vector4<T>::GetMidPoint(const Vector4<T>& start, const Vector4<T>& end)
{
	Vector4<T> midpoint;
	midpoint.x = (start.x + end.x) / 2.0f;
	midpoint.y = (start.y + end.y) / 2.0f;
	midpoint.z = (start.z + end.z) / 2.0f;
	midpoint.w = (start.w + end.w) / 2.0f;
	return(midpoint);
}


