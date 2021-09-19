#pragma once
#include "Common.h"
#include "Object.h"
#include "Observer.h"

class Camera : public GameObject, public Observer
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
	void BuildPerspectiveFovLHMatrix(Matrix<float, 4, 4>& matrix, const Rect2f& viewport, float screenNear, float screenDepth);
	
	void SetViewport(const Rect2f& _viewPort);
	void SetWidth(float _width) { width = _width; }
	void SetHeight(float _height) { height = _height; }

	const Rect2f& GetViewport() const { return viewport; }
	float GetFov() const { return MathUtils::PI / 4.0f; }
	float GetWidth() const { return width; }
	float GetHeight() const { return height; }
	float GetAspectRatio() const { return width / height; }

	void ProcessEvent(Event& e) override;

	Vec3f GetExtent() const { return Vec3f(0.0f, 0.0f, 0.0f); }

private:
	void MatrixRotationYawPitchRoll(Matrix<float, 3, 3>& Matrix, float, float, float);
	void BuildViewMatrix(Vec3f, Vec3f);


	Rect2f viewport;
	float width;
	float height;
	Matrix<float, 4, 4> viewMatrix;

	Point2i oldMousePoint;

	float xAngle;
	float yAngle;
};

