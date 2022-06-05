#include "Common.h"
#include "Mesh.h"
#include "Texture.h"
#include "Object.h"
#include "Input.h"
#include "Material.h"
#include "RayCast.h"
#include "Logger.h"
#include "BoundingVolume.h"
#include "Renderer.h"
#include "Shader.h"
#include "Light.h"
#include "Camera.h"

void MakeWorldMatrix(const Vec3f& position, const Vec3f& scale, const Vec3f& look, const Vec3f& right, const Vec3f& up, Matrix<float, 4, 4>& worldMatrix) {
	
	//Right
	worldMatrix.value[0] = right.x * scale.x;
	worldMatrix.value[1] = right.y;
	worldMatrix.value[2] = right.z;

	//Up
	worldMatrix.value[4] = up.x;
	worldMatrix.value[5] = up.y * scale.y;
	worldMatrix.value[6] = up.z;

	//Look
	worldMatrix.value[8] = look.x;
	worldMatrix.value[9] = look.y;
	worldMatrix.value[10] = look.z * scale.z;

	//Position
	worldMatrix.value[12] = position.x;
	worldMatrix.value[13] = position.y;
	worldMatrix.value[14] = position.z;
	worldMatrix.value[15] = 1.0f;
}

void MakeWorldViewMatrix(const Matrix<float, 4, 4>& worldMatrix, const Matrix<float, 4, 4>& viewMatrix, Matrix<float, 4, 4>& worldViewMatrix) {
	//modelviewMatrix;
	worldViewMatrix = MathUtils::Multiply(worldMatrix, viewMatrix);
}

void MakeNormalMatrix(const Matrix<float, 4, 4>& worldViewMatrix, Matrix<float, 3, 3>& normalMatrix) {
	Matrix<float, 3, 3> worldViewMatrix3x3 = Truncate(worldViewMatrix);
	normalMatrix = Inverse(worldViewMatrix3x3).Transpose();
}

GameObject::GameObject() 
	: diffuseMap(nullptr), normalMap(nullptr)
{
	transform = AddComponent<RigidTransform>();
}

GameObject::~GameObject() {

}


void GameObject::SetPosition(const Vec3f& _position) {
	transform.get()->SetPosition(_position);
}

void GameObject::SetPosition(float x, float y, float z) {
	SetPosition(Vec3f(x, y, z));
}

Vec3f GameObject::GetPosition() const {
	return transform.get()->GetPosition();
}

void GameObject::SetScale(const Vec3f& _scale) {
	transform.get()->SetScale(_scale);
}

void GameObject::SetScale(float sx, float sy, float sz) {
	SetScale(Vec3f(sx, sy, sz));
}

Vec3f GameObject::GetScale() const {
	return transform.get()->GetScale();
}

Vec3f GameObject::GetLook() const {
	return transform.get()->GetLook();
}

Vec3f GameObject::GetRight() const {
	return transform.get()->GetRight();
}

Vec3f GameObject::GetUp() const {
	return transform.get()->GetUp();
}

void GameObject::Move(const Vec3f& offset) {
	transform->Move(offset);
}

void GameObject::Move(const Vec3f& dir, float movingSpeed, float elapsedTime) {
	transform->SetMovingSpeed(movingSpeed);
	transform->Move(dir, elapsedTime);
}

void GameObject::Rotate(float pitch, float yaw, float roll) {
	transform.get()->Rotate(pitch, yaw, roll);
}

bool GameObject::Initialize(Renderer& renderer) {
	
	return true;
}
void GameObject::Render(Renderer& renderer, ShaderParameter& shaderParam) {
	switch (renderer.GetRenderMode()) {
	case Renderer::RenderMode::SOLID:
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	case Renderer::RenderMode::WIREFRAME:
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	default:
		assert(0);
		break;
	}
}

bool GameObject::Intersect(const Ray& ray, double& distance) {
	for (const auto& mesh : meshes) {
		for (size_t iTriangle = 0; iTriangle < mesh->GetTriangleMeshCount(); iTriangle++) {
			Triangle triangleMesh = mesh->GetTriangleMesh(iTriangle);
			Vec4f vertex0 = ::Transform(transform->GetWorldMatrix(), Vec4f(triangleMesh.vertices[0].position, 1.0f));
			Vec4f vertex1 = ::Transform(transform->GetWorldMatrix(), Vec4f(triangleMesh.vertices[1].position, 1.0f));
			Vec4f vertex2 = ::Transform(transform->GetWorldMatrix(), Vec4f(triangleMesh.vertices[2].position, 1.0f));

			if (IntersectTriangle(ray, Vec3f(vertex0.x, vertex0.y, vertex0.z), Vec3f(vertex1.x, vertex1.y, vertex1.z), Vec3f(vertex2.x, vertex2.y, vertex2.z), distance)) {
				return true;
			}
		}
	}
	
	return false;
}

void GameObject::FillShaderParameter(ShaderParameter& shaderParam, const Matrix<float, 4, 4>& viewMatrix, const Matrix<float, 4, 4>& projectionMatrix, const Light& light, const Camera& Camera, int iObj) {
	Matrix<float, 4, 4> worldMatrix = Matrix<float, 4, 4>::Identity();
	MakeWorldMatrix(GetPosition(), GetScale(), GetLook(), GetRight(), GetUp(), worldMatrix);
	Matrix<float, 4, 4> worldViewMatrix = Matrix<float, 4, 4>::Identity();
	Matrix<float, 3, 3> normalMatrix = Matrix<float, 3, 3>::Identity();
	MakeWorldViewMatrix(worldMatrix, viewMatrix, worldViewMatrix);
	MakeNormalMatrix(worldViewMatrix, normalMatrix);

	shaderParam.worldMatrix = worldMatrix;
	shaderParam.worldViewMatrix = worldViewMatrix;
	shaderParam.viewMatrix = viewMatrix;
	shaderParam.projectionMatrix = projectionMatrix;
	shaderParam.normalMatrix = normalMatrix;

	shaderParam.lightPosition = light.GetPosition();
	shaderParam.diffuse = material->GetDiffuse();
	shaderParam.ambient = material->GetAmbient();
	shaderParam.specular = material->GetSpecular();

	shaderParam.cameraPosition = Camera.GetPosition();
	shaderParam.objNo = iObj;

	if (material->DoesHaveTexture(Material::TextureType::TEXTURE_DIFFUSE)) {
		shaderParam.diffuseTexture = material->GetTextureUnit(Material::TextureType::TEXTURE_DIFFUSE);
	}
	if (material->DoesHaveTexture(Material::TextureType::TEXTURE_NORMAL)) {
		shaderParam.normalTexture = material->GetTextureUnit(Material::TextureType::TEXTURE_NORMAL);
	}
}

void GameObject::GetWorldBoundingBox(Volumef& volume) const{
	GetLocalBoundingBox(volume);
	
	Vec3f worldPosition = GetPosition();
	volume.Move(worldPosition);
}

void GameObject::GetLocalBoundingBox(Volumef& volume) const{
	if (!volume.IsZero())
		return;

	volume = boundingVolume->GetVolume();
}

bool IntersectTriangle(const Ray& ray, const Vec3f& v0, const Vec3f& v1, const Vec3f& v2, bool bFrontOnly, double& distance) {
	Vec3f edge1 = v1 - v0;
	Vec3f edge2 = v2 - v0;

	Vec3f pVec = ray.GetDirection().Cross(edge2);

	float det = edge1.DotProduct(pVec);

	Vec3f tVec;
	if (det > 0) {
		tVec = v0 - ray.GetPosition();
	}
	else {
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

bool IntersectTriangle(const Ray& ray, const Vec3f& v0, const Vec3f& v1, const Vec3f& v2, double& distance) {
	auto e1 = v1 - v0, e2 = v2 - v0, h = Cross(ray.GetDirection(), e2);
	auto a = DotProduct(e1, h);
	if (std::abs(a) == 0) return false;

	float f = 1 / a;
	auto s = ray.GetPosition() - v0;
	auto u = f * DotProduct(s, h);
	if (u < 0 || u > 1) return false;

	auto q = Cross(s, e1);
	auto v = f * DotProduct(ray.GetDirection(), q);
	if (v < 0 || u + v > 1) return false;

	auto t = f * DotProduct(e2, q);
	if (t < 0) return false;

	distance = t;

	return true;
}

bool IntersectTriangle(const Ray& ray, const Triangle& triangle, double& distance) {
	Vec3f v0 = triangle.GetPosition(0);
	Vec3f v1 = triangle.GetPosition(1);
	Vec3f v2 = triangle.GetPosition(2);

	auto e1 = v1 - v0;
	auto e2 = v2 - v0;
	auto h = Cross(ray.GetDirection(), e2);

	auto a = DotProduct(e1, h);
	if (std::abs(a) == 0)
		return false;

	float f = 1 / a;
	auto s = ray.GetPosition() - v0;
	auto u = f * DotProduct(s, h);
	if (u < 0 || u > 1) return false;

	auto q = Cross(s, e1);
	auto v = f * DotProduct(ray.GetDirection(), q);
	if (v < 0 || u + v > 1) return false;

	auto t = f * DotProduct(e2, q);
	if (t < 0) return false;

	distance = t;

	return true;
}


///////////////////////////////////////////////////////////////
Cube::Cube()
	:GameObject()
{

}

Cube::Cube(const Vec3f& size)
	: GameObject()
{
	SetScale(size);
}

Cube::~Cube() {

}

bool Cube::Initialize(Renderer& renderer) {
	GameObject::Initialize(renderer);

	AddComponent<PhongShader>();
	shader = GetComponent<Shader>();
	//shader = std::make_shared<TextureShader>(this);
	if (!shader)
		return false;

	if (!shader->Initialize(renderer)) {
		LogError(L"Could not initialize the Default Shader\n");
		return false;
	}

	//MeshBuilder Call
	MeshBuilder meshBuilder;
	meshes.push_back(std::make_shared<Mesh>());

	meshBuilder.Begin();
	meshBuilder.AddCube(transform.get()->GetPosition(), Vec3f(1.0f, 1.0f, 1.0f), RGBA::BLUE);
	meshBuilder.CopyToMesh(renderer, *meshes.back(), &Vertex::BindVertexBuffer, &Vertex::Copy, sizeof(Vertex));
	meshBuilder.End();

	diffuseMap = TextureLoader::GetTexture(renderer, L"·¹µåº§ºª-Feel My Rythm.mp4");
	
	renderer.AllocateTextures(diffuseMap->textureID, 1);
	renderer.BindTexture(diffuseMap->GetTextureID());
	renderer.SetImage(GL_TEXTURE_2D, diffuseMap->GetPicture().GetMemory(), diffuseMap->GetPicture().GetWidth(), diffuseMap->GetPicture().GetHeight());
	renderer.SetSampleMode();
	renderer.SetFiltering();

	//normalMap = TextureLoader::GetTexture(renderer, "Resource\\Texture\\BMP\\NormalMap.bmp");
	
	Vec3f diffuseColor(0.8f, 0.85f, 0.85f);
	Vec4f ambientColor(0.3f, 0.3f, 0.3f, 1.0f);
	Vec3f specularColor(1.0f, 1.0f, 1.0f);
	material = std::make_shared<Material>(diffuseColor, ambientColor, specularColor, std::make_pair(Material::TextureType::TEXTURE_DIFFUSE, diffuseMap->GetTextureID()));

	AddComponent<BoundingBox>();
	auto boundingBox = GetComponent<BoundingBox>();
	
	boundingBox->SetCenter(transform->GetPosition());
	boundingBox->SetExtent(GetScale());

	boundingVolume = boundingBox;
	if (!boundingVolume->Init(renderer))
		return false;


	return true;
}

void Cube::Shutdown(Renderer& renderer) {
	GameObject::Shutdown(renderer);
	if(shader)
		shader->Shutdown(renderer);

	for (auto& mesh : meshes) {
		mesh->Shutdown(renderer);
	}
}

void Cube::Update(float deltaTime) {
	diffuseMap->Update(deltaTime);
}

void Cube::Render(Renderer& renderer, ShaderParameter& shaderParam) {	
	GameObject::Render(renderer, shaderParam);
	
	shader->Render(renderer, shaderParam);
	renderer.SetDrawMode(Renderer::DrawMode::TRIANGLES);
	renderer.SetDepthTest(true);

	for (auto& mesh : meshes) {
		mesh->Render(renderer);
	}
	boundingVolume->Render(renderer, shaderParam.viewMatrix, shaderParam.projectionMatrix);
}



Sphere::Sphere(float _radius, int _stackCount, int _sectorCount)
	:GameObject(), radius(_radius), stackCount(_stackCount), sectorCount(_sectorCount)
{

}

Sphere::~Sphere() {

}

bool Sphere::Initialize(Renderer& renderer) {
	GameObject::Initialize(renderer);

	shader = std::make_shared<BumpShader>(this);
	if (!shader)
		return false;

	if (!shader->Initialize(renderer)) {
		LogError(L"Could not initialize the Default Shader\n");
		return false;
	}

	//MeshBuilder Call
	MeshBuilder meshBuilder;
	meshes.push_back(std::make_shared<Mesh>());
	meshBuilder.Begin();
	meshBuilder.AddSphere(transform.get()->GetPosition(), 1.0f, sectorCount, stackCount);
	meshBuilder.CopyToMesh(renderer, *meshes.back(), &Vertex::BindVertexBuffer, &Vertex::Copy, sizeof(Vertex));
	meshBuilder.End();

	diffuseMap = TextureLoader::GetTexture(renderer, L"coast_sand_rocks.jpg");
	
	renderer.AllocateTextures(diffuseMap->textureID, 1);
	renderer.BindTexture(diffuseMap->textureID);
	renderer.SetImage(GL_TEXTURE_2D, diffuseMap->GetPicture().GetMemory(), diffuseMap->GetPicture().GetWidth(), diffuseMap->GetPicture().GetHeight());
	renderer.SetSampleMode();
	renderer.SetFiltering();

	normalMap = TextureLoader::GetTexture(renderer, L"concrete_normal.png");
	
	renderer.AllocateTextures(normalMap->textureID, 1);
	renderer.BindTexture(normalMap->textureID);
	renderer.SetImage(GL_TEXTURE_2D, diffuseMap->GetPicture().GetMemory(), normalMap->GetPicture().GetWidth(), normalMap->GetPicture().GetHeight());
	renderer.SetSampleMode();
	renderer.SetFiltering();


	SetScale(radius, radius, radius);
	
	Vec3f diffuseColor(0.8f, 0.85f, 0.85f);
	Vec4f ambientColor(0.3f, 0.3f, 0.3f, 1.0f);
	Vec3f specularColor(1.0f, 1.0f, 1.0f);
	material = std::make_shared<Material>(diffuseColor, ambientColor, specularColor, std::make_pair(Material::TextureType::TEXTURE_DIFFUSE, diffuseMap->GetTextureID()));
	material->SetTextureMap(std::make_pair(Material::TextureType::TEXTURE_NORMAL, normalMap->GetTextureID()));

	AddComponent<BoundingBox>();
	auto boundingBox = GetComponent<BoundingBox>();

	boundingBox->SetCenter(transform->GetPosition());
	boundingBox->SetExtent(GetScale());

	boundingVolume = boundingBox;
	if (!boundingVolume->Init(renderer)) {
		return false;
	}

	return true;
}

void Sphere::Shutdown(Renderer& renderer) {
	GameObject::Shutdown(renderer);
	if (shader)
		shader->Shutdown(renderer);
	for (auto& mesh : meshes) {
		mesh->Shutdown(renderer);
	}
}

void Sphere::Update(float deltaTime) {
	diffuseMap->Update(deltaTime);
}

void Sphere::Render(Renderer& renderer, ShaderParameter& shaderParam) {
	GameObject::Render(renderer, shaderParam);
	shader->Render(renderer, shaderParam);
	renderer.SetDrawMode(Renderer::DrawMode::TRIANGLES);
	renderer.SetDepthTest(true);
	for (auto& mesh : meshes) {
		mesh->Render(renderer);
	}
	boundingVolume->Render(renderer, shaderParam.viewMatrix, shaderParam.projectionMatrix);
}


///////////////////////////////////////////////////
//Skybox
Cubemap::Cubemap()
	: GameObject()
{

}

Cubemap::~Cubemap() {

}

bool Cubemap::Initialize(Renderer& renderer) {
	shader = std::make_shared<SkyboxShader>(this);
	if (!shader)
		return false;

	if (!shader->Initialize(renderer)) {
		LogError(L"Could not initialize the Default Shader\n");
		return false;
	}

	MeshBuilder meshBuilder;
	meshes.push_back(std::make_shared<Mesh>());
	meshBuilder.Begin();
	meshBuilder.AddCube(transform.get()->GetPosition(), Vec3f(100.0f, 100.0f, 100.0f), RGBA::BLUE, false);
	meshBuilder.CopyToMesh(renderer, *meshes.back(), &Vertex::BindVertexBuffer, &Vertex::Copy, sizeof(Vertex));
	meshBuilder.End();

	std::array<std::shared_ptr<Texture>, 6> cubeTextures;
	cubeTextures[0] = TextureLoader::GetTexture(renderer, L"SkyBox_Right.png");
	cubeTextures[1] = TextureLoader::GetTexture(renderer, L"SkyBox_Left.png");
	cubeTextures[2] = TextureLoader::GetTexture(renderer, L"SkyBox_Top.png");
	cubeTextures[3] = TextureLoader::GetTexture(renderer, L"SkyBox_Bottom.png");
	cubeTextures[4] = TextureLoader::GetTexture(renderer, L"SkyBox_Front.png");
	cubeTextures[5] = TextureLoader::GetTexture(renderer, L"SkyBox_Back.png");
	
	unsigned int cubemapTexture;
	renderer.AllocateTextures(cubemapTexture, 1);
	renderer.BindCubemapTexture(cubemapTexture);
	renderer.SetSampleMode(true);
	renderer.SetFiltering(true);

	for (size_t iTexture = 0; iTexture < cubeTextures.size(); iTexture++) {
		Size2u imageSize(cubeTextures[iTexture]->GetPicture().GetWidth(), cubeTextures[iTexture]->GetPicture().GetHeight());
		renderer.SetImage(GL_TEXTURE_CUBE_MAP_POSITIVE_X + iTexture, cubeTextures[iTexture]->GetPicture().GetMemory(), imageSize.width, imageSize.height);
	}
	
	material = std::make_shared<Material>(Vec3f(1.0f, 1.0f, 1.0f), Vec4f(1.0f, 1.0f, 1.0f), Vec3f(1.0f, 1.0f, 1.0f), std::make_pair(Material::TextureType::TEXTURE_DIFFUSE, cubemapTexture));
	
	return true;
}

void Cubemap::Shutdown(Renderer& renderer) {
	GameObject::Shutdown(renderer);
	if (shader)
		shader->Shutdown(renderer);

	for (auto& mesh : meshes) {
		mesh->Shutdown(renderer);
	}
}

void Cubemap::Render(Renderer& renderer, ShaderParameter& shaderParam) {
	GameObject::Render(renderer, shaderParam);
	shader->Render(renderer, shaderParam);
	renderer.SetDrawMode(Renderer::DrawMode::TRIANGLES);
	renderer.SetDepthTest(true);
	for (auto& mesh : meshes) {
		mesh->Render(renderer);
	}
}

///////////////////////////////////////////////////
Cylinder::Cylinder(const Vec3f& _axis, const Vec3f& _arm1, const Vec3f& _arm2, uint32_t slices) 
	: GameObject(), axis(_axis), arm1(_arm1), arm2(_arm2), sliceCount(slices) {

}

Cylinder::~Cylinder() {

}


bool Cylinder::Initialize(Renderer& renderer) {
	GameObject::Initialize(renderer);

	shader = std::make_shared<PhongShader>(this);
	if (!shader)
		return false;

	if (!shader->Initialize(renderer)) {
		LogError(L"Could not initialize the Default Shader\n");
		return false;
	}

	//MeshBuilder Call
	MeshBuilder meshBuilder;
	meshes.push_back(std::make_shared<Mesh>());
	meshBuilder.Begin();
	meshBuilder.AddCylinder(axis, arm1, arm2, sliceCount, RGBA::BLUE);
	meshBuilder.CopyToMesh(renderer, *meshes.back(), &Vertex::BindVertexBuffer, &Vertex::Copy, sizeof(Vertex));
	meshBuilder.End();

	diffuseMap = TextureLoader::GetTexture(renderer, L"coast_sand_rocks.jpg");

	renderer.AllocateTextures(diffuseMap->textureID, 1);
	renderer.BindTexture(diffuseMap->textureID);
	renderer.SetImage(GL_TEXTURE_2D, diffuseMap->GetPicture().GetMemory(), diffuseMap->GetPicture().GetWidth(), diffuseMap->GetPicture().GetHeight());
	renderer.SetSampleMode();
	renderer.SetFiltering();

	Vec3f diffuseColor(0.8f, 0.85f, 0.85f);
	Vec4f ambientColor(0.3f, 0.3f, 0.3f, 1.0f);
	Vec3f specularColor(1.0f, 1.0f, 1.0f);
	material = std::make_shared<Material>(diffuseColor, ambientColor, specularColor, std::make_pair(Material::TextureType::TEXTURE_DIFFUSE, diffuseMap->GetTextureID()));
	
	AddComponent<BoundingBox>();
	auto boundingBox = GetComponent<BoundingBox>();

	boundingBox->SetCenter(transform->GetPosition());
	boundingBox->SetExtent(GetScale());

	boundingVolume = boundingBox;
	if (!boundingVolume->Init(renderer)) {
		return false;
	}

	return true;
}


void Cylinder::Shutdown(Renderer& renderer) {
	GameObject::Shutdown(renderer);
	for (auto& mesh : meshes) {
		mesh->Shutdown(renderer);
	}
}

void Cylinder::Update(float deltaTime) {
	diffuseMap->Update(deltaTime);
}

void Cylinder::Render(Renderer& renderer, ShaderParameter& shaderParam) {
	GameObject::Render(renderer, shaderParam);

	shader->Render(renderer, shaderParam);
	renderer.SetDrawMode(Renderer::DrawMode::TRIANGLES);
	renderer.SetDepthTest(true);

	for (auto& mesh : meshes) {
		mesh->Render(renderer);
	}
	boundingVolume->Render(renderer, shaderParam.viewMatrix, shaderParam.projectionMatrix);
}


GameObjects::GameObjects() {

}

GameObjects::~GameObjects() {

}

void GameObjects::Add(GameObject& gameObj) {
	GameObjects::iterator iObj = std::find(begin(), end(), &gameObj);
	if (iObj != end())
		return;
	
	push_back(&gameObj);
}

bool GameObjects::Clear() {
	while (!empty()) {
		delete back();
		pop_back();
	}
	return true;
}
//std::shared_ptr<GameObject>& ObjectFactory::CreateGameObject(Renderer& renderer, ObjectFactory::eObjectType objType, ObjectFactory::eShaderType shaderType) {
//	MeshBuilder meshBuilder;
//	Vec3f defaultPosition(0.0f, 0.0f, 0.0f);
//	Vec3f defaultExtent(2.0f, 2.0f, 2.0f);
//	switch (objType) {
//	case eObjectType::OBJECT_CUBE:				meshBuilder.AddCube(defaultPosition, defaultExtent,RGBA::BLUE);
//	case eObjectType::OBJECT_SPHERE:			meshBuilder.AddSphere(defaultPosition, 2, 12, 12);
//	case eObjectType::OBJECT_CYLINDER:			meshBuilder.AddCylinder(Vec3f::UP, Vec3f::RIGHT, Vec3f::FORWARD, 32, RGBA::GREEN);
//	}
//
//	std::shared_ptr<GameObject> gameObject = std::make_shared<GameObject>();
//	meshBuilder.CopyToMesh(renderer, gameObject->GetMesh(), &Vertex::BindVertexBuffer, &Vertex::Copy, sizeof(Vertex));
//
//	switch (shaderType) {
//	case eShaderType::SHADER_COLOR: 	gameObject->AddComponent<ColorShader>();
//	case eShaderType::SHADER_TEXTURE:   gameObject->AddComponent<TextureShader>();
//	case eShaderType::SHADER_PHONG:		gameObject->AddComponent<PhongShader>();
//	}
//	
//	gameObject->shader = gameObject->GetComponent<Shader>();
//	return gameObject;
//}