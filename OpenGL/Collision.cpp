#include "Collision.h"
#include "Object.h"
#include "RayCast.h"


CollisionDetector::CollisionDetector() {

}

CollisionDetector::~CollisionDetector() {

}

bool CollisionDetector::DetectRayCollision(const Ray& ray, const GameObject& obj) {

	return true;
}

//bool CollisionDetector::DetectIntersection(const Ray& ray, const Vec3f& posX, const Vec3f& posY, const Vec3f& posZ, double& distance) {
//
//	return true;
//}

CollisionResolver::CollisionResolver() {

}

CollisionResolver::~CollisionResolver() {

}
