#pragma once

class Ray;
class GameObject;

//충돌 감지 객체
class CollisionDetector {
public:
	CollisionDetector();
	~CollisionDetector();

	bool DetectRayCollision(const Ray& ray, const GameObject& obj);
	//bool DetectIntersection(const Ray& ray, const Vec3f& posX, const Vec3f& posY, const Vec3f& posZ, double& distance);
};


/// <summary>
/// 충돌 해결 객체
/// </summary>
class CollisionResolver {
	CollisionResolver();
	~CollisionResolver();
};