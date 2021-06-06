#pragma once
#include <memory>
#include "Matrix.h"

class OpenGL;
class Mesh;
class Texture;
class Renderer;
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

	void SetRotation(float x, float y, float z);
	Vec3f GetRotation() const;

	void Move(const Vec3f& direction,float elapsedTime);

	Vec3f GetLook() const;
	Vec3f GetRight() const;
	Vec3f GetUp() const;
protected:
	virtual bool CheckIfNeedToKeyProcess() const;

	Vec3f position;
	Vec3f rotation;

	Matrix<float, 4, 4> worldMatrix;
	float movingSpeed;
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
protected:
	bool CheckIfNeedToKeyProcess() const override;

private:

	std::shared_ptr<Mesh> pMesh;
	std::shared_ptr<Texture> texture;
};

