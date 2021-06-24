#pragma once

class Ray;
class Object;

//�浹 ���� ��ü
class CollisionDetector {
public:
	CollisionDetector();
	~CollisionDetector();

	bool DetectRayCollision(const Ray& ray, const Object& obj);
	//bool DetectIntersection(const Ray& ray, const Vec3f& posX, const Vec3f& posY, const Vec3f& posZ, double& distance);
};


/// <summary>
/// �浹 �ذ� ��ü
/// </summary>
class CollisionResolver {
	CollisionResolver();
	~CollisionResolver();
};