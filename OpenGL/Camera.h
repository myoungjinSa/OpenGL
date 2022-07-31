#pragma once
#include "Common.h"
#include "Types.h"
#include "Object.h"
#include "Observer.h"


class Camera : public GameObject, Observer
{
public:
	Camera();
	Camera(const Camera& other) = delete;
	~Camera();

	bool Initialize(Renderer& renderer) override;
	void Shutdown(Renderer& renderer) override;
	void Update(float deltaTime) override;
	void Render(Renderer& renderer, ShaderParameter& shaderParam)override {}

	void GetViewMatrix(Matrix<float, 4, 4>& Matrix) const;
	void SetFrustum(float width, float height, float _near, float _far, float fov);
	void SetViewport(const Recti& _viewPort);
	Matrix<float, 4, 4> GetFrustum() const { return frustum; }
	
	const Viewporti& GetViewport() const { return viewport; }
	float GetFov() const { return MathUtils::PI / 4.0f; }
	void SetNear(float _Near) { Near = _Near; }
	float GetNear() const { return Near; }
	void SetFar(float _Far) { Far = _Far; }
	float GetFar() const { return Far; }
	float GetAspectRatio() const { return viewport.GetWidth() / viewport.GetHeight(); }
	
	Vec3f GetRight() const;
	Vec3f GetUp() const;
	Vec3f GetLook() const;

	void ProcessEvent(Event& e) override;

private:
	void BuildViewMatrix();

	Viewporti viewport;
	Matrix<float, 4, 4> viewMatrix;
	Matrix<float, 4, 4> frustum;
	
	float Near;
	float Far;

	float xAngle, yAngle;
};

