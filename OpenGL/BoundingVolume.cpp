#include "BoundingVolume.h"
#include "Renderer.h"
#include "Object.h"

BoundingVolume::BoundingVolume(Object* pObject)
	: Component(pObject)
{

}



BoundingBox::BoundingBox(Object* pOwner, const Vec3f& center, const Vec3f& extent) 
	:BoundingVolume(pOwner), center(), extent()
{

}


bool BoundingBox::IsIn(const Vec3f& pos) {


	return true;
}

void BoundingBox::Render(Renderer& renderer) {
	renderer.SetDrawMode(Renderer::DrawMode::LINES);


}

BoundingSphere::BoundingSphere(Object* pOwner, const Vec3f& _center, float size) 
	: BoundingVolume(pOwner), center(_center), radius(size)
{

}

bool BoundingSphere::IsIn(const Vec3f& pos) {

	return true;
}

void BoundingSphere::Render(Renderer& renderer) {
	renderer.SetDrawMode(Renderer::DrawMode::LINES);

	
}