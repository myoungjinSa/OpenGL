#pragma once
#include "Common.h"
#include "Types.h"
#include "Object.h"
#include "Observer.h"

class Viewport {
public:
	Viewport();
	Viewport(const Rect2f& rect);

	void SetViewport(const Rect2f& rect);
	float GetWidth() const { return rect.GetWidth(); }
	float GetHeight() const { return rect.GetHeight(); }

	Rect2f GetRect() const { return rect; }
private:
	Rect2f rect;

	float halfWidth;
	float halfHeight;
	float centerX;
	float centerY;
};
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
	void BuildPerspectiveFovLHMatrix(Matrix<float, 4, 4>& matrix);
	
	void SetViewport(const Rect2f& _viewPort);

	const Viewport& GetViewport() const { return viewport; }
	float GetFov() const { return MathUtils::PI / 4.0f; }
	void SetNear(float _Near) { Near = _Near; }
	float GetNear() const { return Near; }
	void SetFar(float _Far) { Far = _Far; }
	float GetFar() const { return Far; }
	float GetAspectRatio() const { return viewport.GetWidth() / viewport.GetHeight(); }
	
	void ProcessEvent(Event& e) override;

private:
	void MatrixRotationYawPitchRoll(Matrix<float, 3, 3>& Matrix, float, float, float);
	void BuildViewMatrix(Vec3f, Vec3f);


	Viewport viewport;
	Matrix<float, 4, 4> viewMatrix;

	float Near;
	float Far;

	Point2i oldMousePoint;

	float xAngle;
	float yAngle;
};

