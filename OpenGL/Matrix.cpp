#include "Matrix.h"



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


