#pragma once
#include <memory>
#include "Matrix.h"

class Material;
class OpenGL;
class Mesh;
class Texture;
class Renderer;
class Ray;
class Object
{
public:
	Object();
	virtual ~Object() {}
	virtual bool Initialize(Renderer& renderer) { return true; }
	virtual void Shutdown(Renderer& renderer) {}
	virtual void Render(Renderer& renderer) {}
	virtual void Update(float deltaTime){}

	void SetPosition(const Vec3f& position);
	void SetPosition(float x, float y, float z);
	Vec3f GetPosition() const;
	void GetPosition(std::array<float, 4>& position);

	Matrix<float, 3, 3> GetRotationMatrix() const;

	void Move(const Vec3f& direction,float elapsedTime);
	void Rotate(float pitch, float yaw, float roll);

	void SetLook(const Vec3f& look);
	void SetRight(const Vec3f& right);
	void SetUp(const Vec3f& up);

	void SetLook(Vec3f&& look) noexcept;
	void SetRight(Vec3f&& right) noexcept;
	void SetUp(Vec3f&& up) noexcept;

	Vec3f GetLook() const;
	Vec3f GetRight() const;
	Vec3f GetUp() const;

	bool Intersect(const Ray& ray, double& distance);
public:
	std::shared_ptr<Material> material;
protected:
	Vec3f position;
	Vec3f rotation;

	Matrix<float, 4, 4> worldMatrix;
	float movingSpeed;

	std::shared_ptr<Mesh> pMesh;
	std::shared_ptr<Texture> texture;

	bool IntersectTriangle(const Ray& ray, const Vec3f& v0, const Vec3f& v1, const Vec3f& v2, bool bFrontOnly, double& distance);
};

class Cube : public Object {
public:
	Cube();
	Cube(const Cube& other);
	Cube(Cube&& other) noexcept;
	virtual ~Cube();

	bool Initialize(Renderer& renderer) override;
	void Shutdown(Renderer& renderer) override;
	void Render(Renderer& renderer) override;
	void Update(float deltaTime) override;

	Cube& operator=(const Cube& other);
	Cube& operator=(Cube&& other) noexcept;

};

