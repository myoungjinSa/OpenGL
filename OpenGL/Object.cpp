#include "Common.h"
#include "Mesh.h"
#include "Texture.h"
#include "Object.h"
#include "Input.h"
#include "Material.h"
#include "RayCast.h"

Object::Object() 
	: pMesh{ nullptr }, texture{ nullptr }, position(0.0f, 0.0f, 0.0f), rotation(0.0f, 0.0f, 0.0f), movingSpeed(1.0f)
{

}

void Object::SetPosition(const Vec3f& _position) {
	position = _position;
}
void Object::SetPosition(float x, float y, float z) {
	position = Vec3f(x, y, z);
}

Vec3f Object::GetPosition() const {
	return position;
}
void Object::GetPosition(std::array<float, 4>& _position) {
	_position[0] = position.x;
	_position[1] = position.y;
	_position[2] = position.z;
	_position[3] = 1.0f;
}


Matrix<float, 3, 3> Object::GetRotationMatrix() const {
	Matrix<float, 3, 3> rotationMatrix;
	rotationMatrix.value[0] = worldMatrix.value[0];
	rotationMatrix.value[1] = worldMatrix.value[1];
	rotationMatrix.value[2] = worldMatrix.value[2];

	rotationMatrix.value[3] = worldMatrix.value[4];
	rotationMatrix.value[4] = worldMatrix.value[5];
	rotationMatrix.value[5] = worldMatrix.value[6];

	rotationMatrix.value[6] = worldMatrix.value[8];
	rotationMatrix.value[7] = worldMatrix.value[9];
	rotationMatrix.value[8] = worldMatrix.value[10];

	return rotationMatrix;
}



void Object::Rotate(float pitch, float yaw, float roll) {
	float cYaw, cPitch, cRoll, sYaw, sPitch, sRoll;

	cYaw = cosf(yaw);
	cPitch = cosf(pitch);
	cRoll = cosf(roll);

	sYaw = sinf(yaw);
	sPitch = sinf(pitch);
	sRoll = sinf(roll);

	Matrix<float, 4, 4> rotationMatrix;
	//Calculate the yaw, pitch , roll rotation matrix.
	rotationMatrix[0] = (cRoll * cYaw) + (sRoll * sPitch * sYaw);
	rotationMatrix[1] = (sRoll * cPitch);
	rotationMatrix[2] = (cRoll * -sYaw) + (sRoll * sPitch * cYaw);
	rotationMatrix[3] = 0.0f;

	rotationMatrix[4] = (-sRoll * cYaw) + (cRoll * sPitch * sYaw);
	rotationMatrix[5] = (cRoll * cPitch);
	rotationMatrix[6] = (sRoll * sYaw) + (cRoll * sPitch * cYaw);
	rotationMatrix[7] = 0.0f;

	rotationMatrix[8] = (cPitch * sYaw);
	rotationMatrix[9] = -sPitch;
	rotationMatrix[10] = (cPitch * cYaw);
	rotationMatrix[11] = 0.0f;

	rotationMatrix[12] = 0.0f;
	rotationMatrix[13] = 0.0f;
	rotationMatrix[14] = 0.0f;
	rotationMatrix[15] = 1.0f;

	worldMatrix.Multiply(rotationMatrix);
}

void Object::Move(const Vec3f& direction, float elapsedTime) {
	position = direction * elapsedTime * movingSpeed;
}

void Object::SetLook(const Vec3f& look) {
	worldMatrix.value[8] = look.x;
	worldMatrix.value[9] = look.y;
	worldMatrix.value[10] = look.z;
}

void Object::SetLook(Vec3f&& look) noexcept{
	worldMatrix.value[8] = look.x;
	worldMatrix.value[9] = look.y;
	worldMatrix.value[10] = look.z;
}
void Object::SetUp(const Vec3f& up)  {
	worldMatrix.value[0] = up.x;
	worldMatrix.value[1] = up.y;
	worldMatrix.value[2] = up.z;
}


void Object::SetUp(Vec3f&& up) noexcept{
	worldMatrix.value[0] = up.x;
	worldMatrix.value[1] = up.y;
	worldMatrix.value[2] = up.z;
}

void Object::SetRight(const Vec3f& right) {
	worldMatrix.value[4] = right.x;
	worldMatrix.value[5] = right.y;
	worldMatrix.value[6] = right.z;
}

void Object::SetRight(Vec3f&& right) noexcept {
	worldMatrix.value[4] = right.x;
	worldMatrix.value[5] = right.y;
	worldMatrix.value[6] = right.z;
}

Vec3f Object::GetLook() const {
	Vec3f lookVector(worldMatrix.value[8], worldMatrix.value[9], worldMatrix.value[10]);
	return Normalize(lookVector);
}

Vec3f Object::GetRight() const {
	Vec3f rightVector(worldMatrix.value[4], worldMatrix.value[5], worldMatrix.value[6]);
	return Normalize(rightVector);
}

Vec3f Object::GetUp() const {
	Vec3f upVector(worldMatrix.value[0], worldMatrix.value[1], worldMatrix.value[2]);
	return Normalize(upVector);
}

bool Object::Intersect(const Ray& ray, double& distance) {
	bool bIntersect = false;
	for (size_t iTriangle = 0; iTriangle < pMesh->GetTriangleMeshCount(); iTriangle++) {
		Triangle triangleMesh = pMesh->GetTriangleMesh(iTriangle);
		bIntersect = IntersectTriangle(ray, triangleMesh.vertices[0].position, triangleMesh.vertices[1].position, triangleMesh.vertices[2].position, false, distance);
	}
	
	return bIntersect;
}
bool Object::IntersectTriangle(const Ray& ray, const Vec3f& v0, const Vec3f& v1, const Vec3f& v2, bool bFrontOnly, double& distance) {
	Vec3f edge1 = v1 - v0;
	Vec3f edge2 = v2 - v0;

	Vec3f pVec = ray.GetDirection().Cross(edge2);

	float det = edge1.DotProduct(pVec);

	Vec3f tVec;
	if (det > 0) {
		tVec = v0 - ray.GetPosition();
	}else {
		//BackFace 
		if (bFrontOnly)
			return false;

		tVec = v0 - ray.GetPosition();
		det = -det;
	}

	if (det < 0.0001f)
		return false;


	float u = tVec.DotProduct(pVec);
	if (u < 0 || u > det)
		return false;

	Vec3f qVec = tVec.Cross(edge1);

	float v = ray.GetDirection().DotProduct(qVec);
	if (v < 0 || u + v > det)
		return false;

	distance = edge2.DotProduct(qVec) / det;

	return true;
}
///////////////////////////////////////////////////////////////
Cube::Cube()
	:Object()
{
	worldMatrix = Matrix<float, 4, 4>::Identity();
}

Cube::Cube(const Cube& other)
{
	operator=(other);
}

Cube::~Cube() {

}

Cube& Cube::operator=(const Cube& other) {
	if (this == &other)
		return *this;

	if (0 < other.pMesh.use_count()) {
		pMesh = other.pMesh;
	}

	return *this;
}

bool Cube::Initialize(Renderer& renderer) {
	Object::Initialize(renderer);

	//MeshBuilder Call
	MeshBuilder meshBuilder;
	meshBuilder.AddCube(2.0f, RGBA::BLUE);
	if (!pMesh)
		pMesh = std::make_shared<Mesh>();

	if (!pMesh)
		return false;

	meshBuilder.CopyToMesh(renderer, pMesh.get(), &Vertex::Copy, sizeof(Vertex));

	texture = TextureLoader::GetTexture(renderer, "Capture.bmp");

	Vec3f diffuseColor(0.8f, 0.85f, 0.85f);
	Vec4f ambientColor(0.3f, 0.3f, 0.3f, 1.0f);
	Vec3f specularColor(1.0f, 1.0f, 1.0f);
	material = std::make_shared<Material>(diffuseColor, ambientColor, specularColor, texture->textureID);

	return true;
}

void Cube::Shutdown(Renderer& renderer) {
	Object::Shutdown(renderer);
	pMesh->Shutdown(renderer);
}

void Cube::Update(float deltaTime) {

}

void Cube::Render(Renderer& renderer) {
	Object::Render(renderer);
	pMesh->Render(renderer);
}

