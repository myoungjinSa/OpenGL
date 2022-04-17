#pragma once
#include <memory>
#include "Component.h"
#include "Mesh.h"
#include "Types.h"

class Renderer;
class ColorShader;
class Object;
class GameObject;
class ShaderParameter;
class Ray;

class BoundingVolume : public Component{
public:
	BoundingVolume(Object* pOwner);
	virtual ~BoundingVolume() {}
	
	virtual Volumef GetVolume() = 0;
	virtual bool Init(Renderer& renderer);
	virtual bool IsIn(const Ray& ray) = 0;
	virtual bool IsIn(const Vec3f& pos) = 0;
	virtual void Render(Renderer& renderer, const Matrix<float, 4, 4>& viewMatrix, const Matrix<float, 4, 4>& projectionMatrix);
protected:
	std::shared_ptr<ColorShader> DefaultShader;
	GameObject* pGameObject;
};


class BoundingBox final : public BoundingVolume {
public:
	BoundingBox(Object* pOnwer);

	Volumef GetVolume() override;
	bool Init(Renderer& renderer) override;
	bool IsIn(const Vec3f& pos) override;
	bool IsIn(const Ray& ray) override;
	void Render(Renderer& renderer, const Matrix<float, 4, 4>& viewMatrix, const Matrix<float, 4, 4>& projectionMatrix) override;

	void SetCenter(const Vec3f& _center);
	void SetExtent(const Vec3f& _extent);

	const Vec3f& GetExtent() const;
	const Vec3f& GetCenter() const;
private:
	bool IntersectAABB(const Ray& ray);
	void GetMinMaxRange(Vec3f& min, Vec3f& max);
	void GetMinMaxRange(Volumef& volume);

	std::shared_ptr<Mesh> pMesh;
	Vec3f center;
	Vec3f extent;
};


class BoundingSphere final : public BoundingVolume {
public:
	BoundingSphere(Object* pOwner);

	Volumef GetVolume() override;
	bool Init(Renderer& renderer) override;
	bool IsIn(const Vec3f& pos) override;
	bool IsIn(const Ray& ray) override;
	void Render(Renderer& renderer, const Matrix<float, 4, 4>& viewMatrix, const Matrix<float, 4, 4>& projectionMatrix) override;

	void SetRadius(float _radius);
	void SetCenter(const Vec3f& _center);

	float GetRadius() const { return radius; }
	const Vec3f& GetCenter() const { return center;	}
private:
	void GetMinMaxRange(Vec3f& min, Vec3f& max);
	void GetMinMaxRange(Volumef& volume);

	Vec3f center;
	float radius;
};
