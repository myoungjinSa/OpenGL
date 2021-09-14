#include "Quaternion.h"

Vector4<double> Quaternion::Multiply(const Vector4<double>& left, const Vector4<double>& right) {
	return Vector4<double>(left.x * right.w + left.w * right.x + left.y * right.z - left.z * right.y, left.y * right.w + left.w * right.y + left.z * right.x - left.x * right.z, left.z * right.w + left.w * right.z + left.x * right.y - left.y * right.x, left.w * right.w - left.x * right.x - left.y * right.y - left.z * right.z );
}

Vector3<double> Quaternion::GetXDirection(const Vector4<double>& qVec) {
	return Vector3<double> (qVec.w * qVec.w + qVec.x * qVec.x - qVec.y * qVec.y - qVec.z * qVec.z, (qVec.x * qVec.y + qVec.z * qVec.w) * 2, (qVec.z * qVec.x - qVec.y * qVec.w) * 2);
}
Vector3<double> Quaternion::GetYDirection(const Vector4<double>& qVec) {
	return Vector3<double>((qVec.x * qVec.y - qVec.z * qVec.w) * 2, qVec.w * qVec.w - qVec.x * qVec.x + qVec.y * qVec.y - qVec.z * qVec.z, (qVec.y * qVec.z + qVec.x * qVec.w) * 2);
}

Vector3<double> Quaternion::GetZDirection(const Vector4<double>& qVec) {
	return Vector3<double>((qVec.z * qVec.x + qVec.y * qVec.w) * 2, (qVec.y * qVec.z - qVec.x * qVec.w) * 2, qVec.w * qVec.w - qVec.x * qVec.x - qVec.y * qVec.y + qVec.z * qVec.z);
}


Vector3<double> Quaternion::Rotate(const Vector4<double>& q, const Vector3<double>& v) {
	return Vector3<double>(GetXDirection(q) * v.x + GetYDirection(q) * v.y + GetZDirection(q) * v.z);
}