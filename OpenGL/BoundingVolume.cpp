#include "BoundingVolume.h"
#include "Renderer.h"
#include "Object.h"

BoundingVolume::BoundingVolume(Object* pObject)
	: Component(pObject)
{

}



BoundingBox::BoundingBox(Object* pOwner)
	:BoundingVolume(pOwner), pMesh(nullptr)
{
	center = pOwner->GetPosition();
	extent = pOwner->GetExtent();
}

bool BoundingBox::Init(Renderer& renderer) {
	MeshBuilder meshBuilder;
	meshBuilder.AddCube(center, extent, RGBA::GREEN);

	if (!pMesh)
		pMesh = std::make_shared<Mesh>();

	if (!pMesh)
		return false;

	meshBuilder.CopyToMesh(renderer, pMesh.get(), &ColorVertex::BindVertexBuffer, &ColorVertex::Copy, sizeof(ColorVertex));

}
bool BoundingBox::IsIn(const Vec3f& pos) {

	return true;
}


void BoundingBox::Render(Renderer& renderer) {
	renderer.SetDrawMode(Renderer::DrawMode::LINES);
	pMesh->Render(renderer);
}

BoundingSphere::BoundingSphere(Object* pOwner, const Vec3f& _center, float size) 
	: BoundingVolume(pOwner), center(_center), radius(size)
{

}

bool BoundingSphere::Init(Renderer& renderer) {

	return true;
}

bool BoundingSphere::IsIn(const Vec3f& pos) {

	return true;
}

void BoundingSphere::Render(Renderer& renderer) {
	renderer.SetDrawMode(Renderer::DrawMode::LINES);

}