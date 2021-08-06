#pragma once
#include <memory>
#include <vector>
#include "Matrix.h"
#include "Component.h"
#include "Transform.h"

class Material;
class OpenGL;
class Mesh;
class Texture;
class Renderer;
class Ray;
class Object
{
public:
	explicit Object();
	virtual ~Object();
	virtual bool Initialize(Renderer& renderer);
	virtual void Shutdown(Renderer& renderer) {}
	virtual void Render(Renderer& renderer) {}
	virtual void Update(float deltaTime){}

	bool Intersect(const Ray& ray, double& distance);

	template<typename T> std::shared_ptr<T> AddComponent() {
		static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");
		for (auto& component : components) {
			if (std::dynamic_pointer_cast<T>(component)) {
				return std::dynamic_pointer_cast<T>(component);
			}
		}

		std::shared_ptr<T> newComponent = std::make_shared<T>(this);
		components.push_back(newComponent);
		return newComponent;
	}
	template<typename T> std::shared_ptr<T> GetComponent() {
		static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");

		for (auto& component : components) {
			if (std::dynamic_pointer_cast<T>(component)) {
				return std::dynamic_pointer_cast<T>(component);
			}
		}
		return nullptr;
	}
	void SetPosition(float x, float y, float z);
	void SetPosition(const Vec3f& _position);
	Vec3f GetPosition() const;

	void SetRight(const Vec3f& right);
	void SetUp(const Vec3f& up);
	void SetLook(const Vec3f& look);
	Vec3f GetLook() const;
	Vec3f GetRight() const;
	Vec3f GetUp() const;

	void Move(const Vec3f& dir, float movingSpeed, float elapsedTime);
	void Rotate(float pitch, float yaw, float roll);
public:
	std::shared_ptr<class Transform> transform;
	std::shared_ptr<Material> material;
protected:
	std::vector<std::shared_ptr<Component>> components;

	std::shared_ptr<Mesh> pMesh;
	std::shared_ptr<Texture> albedoMap;
	std::shared_ptr<Texture> normalMap;

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


class Sphere : public Object {
public:
	explicit Sphere(float _radius, int _stackCount, int _sectorCount);
	Sphere(const Sphere& other);
	Sphere(Sphere&& other) noexcept;
	~Sphere() override;

	bool Initialize(Renderer& renderer) override;
	void Shutdown(Renderer& renderer) override;
	void Render(Renderer& renderer) override;
	void Update(float deltaTime) override;

	Sphere& operator=(const Sphere& other);
	Sphere& operator=(Sphere&& other) noexcept;

private:
	float radius;
	int stackCount;
	int sectorCount;
};