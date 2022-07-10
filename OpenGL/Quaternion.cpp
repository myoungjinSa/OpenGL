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

Quaternion Quaternion::ConvertRotationMatrixToQuaternion(const Matrix<double, 3, 3>& rotationMatrix) {
	Quaternion q;
	double trace = rotationMatrix.value[0] + rotationMatrix.value[4] + rotationMatrix.value[8];

	if (0.0 < trace) {
		double s = 0.5 / sqrt(trace + 1.0);
		q.value[3] = 0.25 / s;
		q.value[0] = (rotationMatrix.value[7] - rotationMatrix.value[5]) * s;
		q.value[1] = (rotationMatrix.value[2] - rotationMatrix.value[6]) * s;
		q.value[2] = (rotationMatrix.value[3] - rotationMatrix.value[1]) * s;
	}
	else {
		if (rotationMatrix.value[4] < rotationMatrix.value[0] && rotationMatrix.value[8] < rotationMatrix.value[0]) {
			double s = 2.0 * sqrt(1.0 + rotationMatrix.value[0] - rotationMatrix.value[4] - rotationMatrix.value[8]);
			q.value[3] = (rotationMatrix.value[7] - rotationMatrix.value[5]) / s;
			q.value[0] = 0.25 * s;
			q.value[1] = (rotationMatrix.value[1] + rotationMatrix.value[3]) / s;
			q.value[2] = (rotationMatrix.value[2] + rotationMatrix.value[6]) / s;
		}
		else if (rotationMatrix.value[8] < rotationMatrix.value[4]) {
			double s = 2.0 * sqrt(1.0 + rotationMatrix.value[4] - rotationMatrix.value[0] - rotationMatrix.value[8]);
			q.value[3] = (rotationMatrix.value[2] - rotationMatrix.value[6]) / s;
			q.value[0] = (rotationMatrix.value[1] + rotationMatrix.value[3]) / s;
			q.value[1] = 0.25 * s;
			q.value[2] = (rotationMatrix.value[5] + rotationMatrix.value[7]) / s;
		}
		else {
			double s = 2.0 * sqrt(1.0 + rotationMatrix.value[8] - rotationMatrix.value[0] - rotationMatrix.value[4]);

			q.value[3] = (rotationMatrix.value[3] - rotationMatrix.value[1]) / s;
			q.value[0] = (rotationMatrix.value[2] + rotationMatrix.value[3]) / s;
			q.value[1] = (rotationMatrix.value[5] + rotationMatrix.value[7]) / s;
			q.value[2] = 0.25 * s;
		}
	}
	return q;
}