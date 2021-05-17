#pragma once
#include <memory>
#include "Matrix.h"
class OpenGL;
class Mesh;
class Texture;

class Object
{
public:
	Object();
	virtual ~Object() {}
	virtual bool Initialize(OpenGL& pGL) { return true; }
	virtual void Shutdown(OpenGL& pGL) {}
	virtual void Render(OpenGL& pGL) {}
	virtual void Update(float deltaTime){}

	void SetPosition(float x, float y, float z);
	Vec3f GetPosition() const;
	void GetPosition(std::array<float, 4>& position);

	void SetRotation(float x, float y, float z);
	Vec3f GetRotation() const;
protected:
	Vec3f position;
	Vec3f rotation;
};

class Cube : public Object {
public:
	Cube();
	Cube(const Cube& other);
	Cube(Cube&& other) noexcept;
	virtual ~Cube();

	bool Initialize(OpenGL& gl) override;
	void Shutdown(OpenGL& gl) override;
	void Render(OpenGL& gl) override;
	void Update(float deltaTime = 0.0f) override;

	Cube& operator=(const Cube& other);
	Cube& operator=(Cube&& other) noexcept;

private:
	std::shared_ptr<Mesh> pMesh;
	std::shared_ptr<Texture> texture;
};

