#pragma once
#include "Component.h"
#include "Matrix.h"

class Object;
class BoundingVolume : public Component{
public:
	BoundingVolume(Object* pOwner);
	virtual bool IsIn(const Vec3f& pos) {}
};


class BoundingBox : public BoundingVolume {
public:
	BoundingBox(Object* pOnwer, const Vec3f& center, const Vec3f& extent);

	bool IsIn(const Vec3f& pos) override;
};


class BoundingSphere : public BoundingVolume {
public:
	BoundingSphere(Object* pOwner, const Vec3f& center, float size);

	bool IsIn(const Vec3f& pos) override;
};
