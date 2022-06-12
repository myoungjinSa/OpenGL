#include "Matrix.h"

double GetDeterminant(const Matrix<float, 2, 2>& other){
	return (double)(other.value[0] * other.value[3] - other.value[1] * other.value[2]);
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


double GetDeterminant(const Matrix<float, 4, 4>& other) {
	double det = 0.0;
	int indexI = 1;
	for (int iCol = 1; iCol < other.cols + 1; iCol++) {
		if ((indexI + iCol) % 2 == 0) {
			det += (double)other.value[(indexI - 1) + (iCol - 1)] * GetDeterminant(GetCofactor(other, indexI, iCol));
		}else {
			det -= (double)other.value[(indexI - 1) + (iCol - 1)] * GetDeterminant(GetCofactor(other, indexI, iCol));
		}
	}

	return det;
}

Matrix<float, 2, 2> Inverse(const Matrix<float, 2, 2>& mat) {
	double det = GetDeterminant(mat);

	if (det == 0)
		return Matrix<float, 2, 2>::Identity();

	Matrix<float, 2, 2> inversedMatrix;
	inversedMatrix.value[0] = mat.value[3] / det;
	inversedMatrix.value[1] = -mat.value[1] / det;
	inversedMatrix.value[2] = -mat.value[2] / det;
	inversedMatrix.value[3] = mat.value[0] / det;

	return inversedMatrix;
}

Matrix<float, 3, 3> Inverse(const Matrix<float, 3, 3>& mat) {
	double det = GetDeterminant(mat);

	if (det == 0)
		return Matrix<float, 3, 3>::Identity();

	Matrix<float, 3, 3> inversedMatrix;

	for (int iRow = 1; iRow < mat.rows + 1; iRow++){
		for (int iCol = 1; iCol < mat.cols + 1; iCol++) {
			if ((iRow + iCol) % 2 == 0)
				inversedMatrix.value[(iRow - 1) + ((iCol - 1) * 3)] = GetDeterminant(GetCofactor(mat, iRow, iCol)) / det;
			else
				inversedMatrix.value[(iRow - 1) + ((iCol - 1) * 3)] = -1.0 * GetDeterminant(GetCofactor(mat, iRow, iCol)) / det;
		}
	}

	return inversedMatrix;
}

Matrix<float, 4, 4> Inverse(const Matrix<float, 4, 4>& mat) {
	double det = GetDeterminant(mat);

	if (det == 0)
		return Matrix<float, 4, 4>::Identity();

	Matrix<float, 4, 4> inversedMatrix;
	inversedMatrix = GetAdjMatrix(mat).Transpose() / det;

	return inversedMatrix;
}

Matrix<float, 2, 2> GetCofactor(const Matrix<float, 3, 3>& mat, int iRow, int iCol) {
	Matrix<float, 2, 2> cofactor;

	int k = 0;
	for (int i = 1; i < mat.rows + 1; i++) {
		if (i == iRow)
			continue;
		
		for (int j = 1; j < mat.cols + 1; j++) {
			if (j == iCol)
				continue;

			cofactor[k++] = mat.value[((i - 1) * 3) + (j - 1)];
		}
	}
	return cofactor;
}
Matrix<float, 3, 3> GetCofactor(const Matrix<float, 4, 4>& mat, int iRow, int iCol) {
	Matrix<float, 3, 3> cofactor;

	int k = 0;
	for (int i = 1; i < mat.rows + 1; i++) {
		if (i == iRow)
			continue;

		for (int j = 1; j < mat.cols + 1; j++) {
			if (j == iCol)
				continue;

			cofactor[k++] = mat.value[((i - 1) * 4) + (j - 1)];
		}
	}

	return cofactor;
}
Matrix<float, 4, 4> GetAdjMatrix(const Matrix<float, 4, 4>& mat) {
	Matrix<float, 4, 4> adjMatrix;

	for (int iRow = 1; iRow < mat.rows + 1; iRow++) {
		for (int iCol = 1; iCol < mat.cols + 1; iCol++) {
			if ((iRow + iCol) % 2 == 0)
				adjMatrix.value[((iRow - 1) * 4) + (iCol - 1)] = GetDeterminant(GetCofactor(mat, iRow, iCol));
			else
				adjMatrix.value[((iRow - 1) * 4) + (iCol - 1)] = -1.0f * GetDeterminant(GetCofactor(mat, iRow, iCol));
		}
	}

	return adjMatrix;
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


