#pragma once
#include "Common.h"
#include "Object.h"

class Camera : public Object
{
public:
	Camera();
	Camera(const Camera& other) = delete;
	~Camera();



	bool Initialize(Renderer& renderer) override;
	void Render(Renderer& renderer) override;
	void Shutdown(Renderer& renderer) override;
	void Update(float deltaTime = 0.0f) override;
	void GetViewMatrix(Matrix<float, 4, 4>& Matrix) const;
private:
	void MatrixRotationYawPitchRoll(Matrix<float, 3, 3>& Matrix, float, float, float);
	void TransformCoord(Vec3f& Vec, Matrix<float, 3, 3>& Matrix);
	void BuildViewMatrix(Vec3f, Vec3f, Vec3f);
	
	Matrix<float, 4, 4> viewMatrix;
};

