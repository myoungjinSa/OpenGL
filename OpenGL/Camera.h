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
	void Update(float deltaTime) override;

	void GetViewMatrix(Matrix<float, 4, 4>& Matrix) const;
	void BuildPerspectiveFovLHMatrix(Matrix<float, 4, 4>& matrix, float screenNear, float screenDepth);
	
	void SetWidth(float _width) { width = _width; }
	void SetHeight(float _height) { height = _height; }
	float GetFov() const { return MathUtils::PI / 4.0f; }
	float GetWidth() const { return width; }
	float GetHeight() const { return height; }
	float GetAspectRatio() const { return width / height; }
private:
	void MatrixRotationYawPitchRoll(Matrix<float, 3, 3>& Matrix, float, float, float);
	void TransformCoord(Vec3f& Vec, Matrix<float, 3, 3>& Matrix);
	void BuildViewMatrix(Vec3f, Vec3f, Vec3f);
	
	float fov;
	float width;
	float height;
	Matrix<float, 4, 4> viewMatrix;
};

