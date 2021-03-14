#pragma once

#include<math.h>
#include "Matrix.h"

class Camera
{
public:
	
private:
	Vec3f position;
	Vec3f rotation;
	Matrix<float, 4, 4> viewMatrix;
};

