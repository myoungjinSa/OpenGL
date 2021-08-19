#pragma once
#include <memory>
#include "Component.h"
#include "Mesh.h"
#include "Matrix.h"

class Renderer;
class Object;
class BoundingVolume : public Component{
public:
	BoundingVolume(Object* pOwner);
	virtual ~BoundingVolume() {}
	
	virtual bool Init(Renderer& renderer) = 0;
	virtual bool IsIn(const Vec3f& pos) = 0;
	virtual void Render(Renderer& renderer) = 0;
};


class BoundingBox final : public BoundingVolume {
public:
	BoundingBox(Object* pOnwer, const Vec3f& _center, const Vec3f& _extent);

	bool Init(Renderer& renderer) override;
	bool IsIn(const Vec3f& pos) override;
	void Render(Renderer& renderer) override;
private:
	std::shared_ptr<Mesh> pMesh;
	Vec3f center;
	Vec3f extent;
};


class BoundingSphere final : public BoundingVolume {
public:
	BoundingSphere(Object* pOwner, const Vec3f& center, float size);

	bool Init(Renderer& renderer) override;
	bool IsIn(const Vec3f& pos) override;
	void Render(Renderer& renderer) override;
private:
	Vec3f center;
	float radius;
};
