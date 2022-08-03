#include "Common.h"
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


void MakeWorldViewMatrix(const Matrix<float, 4, 4>& worldMatrix, const Matrix<float, 4, 4>& viewMatrix, Matrix<float, 4, 4>& worldViewMatrix) {
	//modelviewMatrix;
	worldViewMatrix = MathUtils::Multiply(worldMatrix, viewMatrix);
}

void MakeNormalMatrix(const Matrix<float, 4, 4>& worldViewMatrix, Matrix<float, 3, 3>& normalMatrix) {
	Matrix<float, 3, 3> worldViewMatrix3x3 = worldViewMatrix.Truncate<3, 3>(0, 0);
	normalMatrix = Inverse(worldViewMatrix3x3).Transpose();
}

GameObject::GameObject() 
	: diffuseMap(nullptr), normalMap(nullptr), environMap(nullptr)
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

void GameObject::SetOrientation(const Quaternion& q) {
	transform.get()->SetOrientation(q.GetVector());
}

Quaternion GameObject::GetOrientation() const{
	return Quaternion(transform.get()->GetOrientation());
}

void GameObject::SetScale(const Vec3f& _scale) {
	if (_scale.IsZero())
		return;

	transform.get()->SetScale(_scale);
}

void GameObject::SetScale(float sx, float sy, float sz) {
	SetScale(Vec3f(sx, sy, sz));
}

Vec3f GameObject::GetScale() const {
	return transform.get()->GetScale();
}

void GameObject::SetLookRightUp(const Vec3f& look, const Vec3f& right, const Vec3f& up) {
	transform.get()->SetLookRightUp(look, right, up);
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
	transform->Rotate(pitch, yaw, roll);
}

void GameObject::Rotate(const Quaternion& q) {
	transform->Rotate(q);
}

void GameObject::Rotate(const Matrix<float, 3, 3>& rotationMatrix) {
	transform->Rotate(rotationMatrix);
}


bool GameObject::Initialize(Renderer& renderer) {
	
	return true;
}
void GameObject::Render(Renderer& renderer, ShaderParameter& shaderParam) {
	
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
	Matrix<float, 4, 4> worldMatrix = transform->GetWorldMatrix();
	Matrix<float, 4, 4> worldViewMatrix = Matrix<float, 4, 4>::Identity();
	MakeWorldViewMatrix(worldMatrix, viewMatrix, worldViewMatrix);
	
	Matrix<float, 3, 3> normalMatrix = Matrix<float, 3, 3>::Identity();
	MakeNormalMatrix(worldViewMatrix, normalMatrix);


	shaderParam.worldMatrix = worldMatrix;
	shaderParam.worldViewMatrix = worldViewMatrix;
	shaderParam.viewMatrix = viewMatrix;
	shaderParam.projectionMatrix = projectionMatrix;
	shaderParam.normalMatrix = normalMatrix;
	shaderParam.viewInverseMatrix = Inverse(viewMatrix);

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
	if (material->DoesHaveTexture(Material::TextureType::TEXTURE_ENVIORNMENT)) {
		shaderParam.environmentTexture = material->GetTextureUnit(Material::TextureType::TEXTURE_ENVIORNMENT);
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

Matrix<float, 4, 4> GameObject::GetWorldMatrix() const {
	if (!transform)
		return Matrix<float, 4, 4>::Identity();

	return transform->GetWorldMatrix();
}

bool IntersectTriangle(const Ray& ray, const Vec3f& v0, const Vec3f& v1, const Vec3f& v2, bool bFrontOnly, double& distance) {
	Vec3f edge1 = v1 - v0;
	Vec3f edge2 = v2 - v0;

	Vec3f pVec = edge2.Cross(ray.GetDirection());

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

	Vec3f qVec = edge1.Cross(tVec);

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

	diffuseMap = TextureLoader::GetTexture(renderer, L"IU-Strawberry moon.mp4");
	if (!renderer.CreateTexture(*diffuseMap, diffuseMap->GetPicture().GetSize(), diffuseMap->GetPicture().GetMemory())) {
		return false;
	}

	renderer.SetSampleMode(false, GL_LINEAR, GL_CLAMP_TO_EDGE);
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

	shader = std::make_shared<PhongShader>(this);
	if (!shader)
		return false;

	if (!shader->Initialize(renderer)) {
		LogError(L"Could not initialize the Default Shader\n");
		return false;
	}
	SetScale(radius, radius, radius);
	
	//MeshBuilder Call
	MeshBuilder meshBuilder;
	meshes.push_back(std::make_shared<Mesh>());
	meshBuilder.Begin();
	meshBuilder.AddSphere(transform.get()->GetPosition(), 1.0f, sectorCount, stackCount);
	meshBuilder.CopyToMesh(renderer, *meshes.back(), &Vertex::BindVertexBuffer, &Vertex::Copy, sizeof(Vertex));
	meshBuilder.End();

	diffuseMap = TextureLoader::GetTexture(renderer, L"brick_d.jpg");
	if (!renderer.CreateTexture(*diffuseMap, diffuseMap->GetPicture().GetSize(), diffuseMap->GetPicture().GetMemory()))
		return false;
	
	renderer.SetSampleMode(false, GL_LINEAR, GL_CLAMP_TO_EDGE);
	
	normalMap = TextureLoader::GetTexture(renderer, L"brick_n.jpg");
	if (!renderer.CreateTexture(*normalMap, normalMap->GetPicture().GetSize(), normalMap->GetPicture().GetMemory()))
		return false;

	renderer.SetSampleMode(false, GL_LINEAR, GL_CLAMP_TO_EDGE);

	std::shared_ptr<CubemapTexture> cubemapTexture = std::make_shared<CubemapTexture>();
	cubemapTexture->LoadTexture(renderer, CubemapTexture::eCubemapSide::CUBEMAP_RIGHT, L"SkyBox_Right.png");
	cubemapTexture->LoadTexture(renderer, CubemapTexture::eCubemapSide::CUBEMAP_LEFT, L"SkyBox_Left.png");
	cubemapTexture->LoadTexture(renderer, CubemapTexture::eCubemapSide::CUBEMAP_TOP, L"SkyBox_Top.png");
	cubemapTexture->LoadTexture(renderer, CubemapTexture::eCubemapSide::CUBEMAP_BOTTOM, L"SkyBox_Bottom.png");
	cubemapTexture->LoadTexture(renderer, CubemapTexture::eCubemapSide::CUBEMAP_FRONT, L"SkyBox_Front.png");
	cubemapTexture->LoadTexture(renderer, CubemapTexture::eCubemapSide::CUBEMAP_BACK, L"SkyBox_Back.png");

	if (!cubemapTexture->Init(renderer))
		return false;
	
	environMap = cubemapTexture;

	Vec3f diffuseColor(0.8f, 0.85f, 0.85f);
	Vec4f ambientColor(0.3f, 0.3f, 0.3f, 1.0f);
	Vec3f specularColor(1.0f, 1.0f, 1.0f);
	material = std::make_shared<Material>(diffuseColor, ambientColor, specularColor, std::make_pair(Material::TextureType::TEXTURE_DIFFUSE, diffuseMap->GetTextureID()));
	material->SetTextureMap(std::make_pair(Material::TextureType::TEXTURE_NORMAL,		normalMap->GetTextureID()));
	material->SetTextureMap(std::make_pair(Material::TextureType::TEXTURE_ENVIORNMENT,	environMap->GetTextureID()));
	
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

	CubemapTexture cubemapTexture;
	cubemapTexture.LoadTexture(renderer, CubemapTexture::eCubemapSide::CUBEMAP_RIGHT, L"SkyBox_Right.png");
	cubemapTexture.LoadTexture(renderer, CubemapTexture::eCubemapSide::CUBEMAP_LEFT, L"SkyBox_Left.png");
	cubemapTexture.LoadTexture(renderer, CubemapTexture::eCubemapSide::CUBEMAP_TOP, L"SkyBox_Top.png");
	cubemapTexture.LoadTexture(renderer, CubemapTexture::eCubemapSide::CUBEMAP_BOTTOM, L"SkyBox_Bottom.png");
	cubemapTexture.LoadTexture(renderer, CubemapTexture::eCubemapSide::CUBEMAP_FRONT, L"SkyBox_Front.png");
	cubemapTexture.LoadTexture(renderer, CubemapTexture::eCubemapSide::CUBEMAP_BACK, L"SkyBox_Back.png");
	if (!cubemapTexture.Init(renderer))
		return false;

	material = std::make_shared<Material>(Vec3f(1.0f, 1.0f, 1.0f), Vec4f(1.0f, 1.0f, 1.0f), Vec3f(1.0f, 1.0f, 1.0f), std::make_pair(Material::TextureType::TEXTURE_DIFFUSE, cubemapTexture.GetTextureID()));
	
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
	if (!renderer.CreateTexture(*diffuseMap, diffuseMap->GetPicture().GetSize(), diffuseMap->GetPicture().GetMemory()))
		return false;

	renderer.SetSampleMode(false, GL_LINEAR, GL_CLAMP_TO_EDGE);


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


RenderTarget::RenderTarget()
	: iFrameBuffer(0)
	, size()
{

}

RenderTarget::~RenderTarget() {
	
}


bool RenderTarget::Create(Renderer& renderer) {
	mesh = std::make_shared<Mesh>();

	MeshBuilder meshBuilder;
	meshBuilder.Begin();
	meshBuilder.AddQuad(Vec3f(0.0f, 0.0f, 0.0f), Vec3f(1.0f, 1.0f, 0.0f), Vec3f::FORWARD * -1.0f);
	meshBuilder.CopyToMesh(renderer, *mesh, &Vertex::BindVertexBuffer, &Vertex::Copy, sizeof(Vertex));
	meshBuilder.End();

	//shader = std::make_shared<PostProcessingShader>(this);
	//if (!shader->Initialize(renderer)) {
	//	return false;
	//}

	return true;
}

void RenderTarget::Render(Renderer& renderer, std::shared_ptr<PostProcessingShader>& shader) {
	renderer.SetViewport(Viewporti(0, 0, size.width, size.height));
	renderer.BindTexture(colorTexture.get()->textureID);
	shader->Render(renderer);
	
	renderer.EnableDepthTest(false); 
	
	int screenTexture = colorTexture.get()->textureID;
	renderer.SetShaderParameter(shader->GetShaderProgram(), screenTexture, String("screenTexture"));
	renderer.SetDrawMode(Renderer::DrawMode::TRIANGLES);
	mesh->Render(renderer);

	renderer.BindTexture(0);

	renderer.EnableDepthTest(true);
}

void RenderTarget::SetSize(const Size2u& _size) {
	size = _size;
}

void RenderTarget::SetSize(size_t width, size_t height) {
	size.width = width;
	size.height = height;
}

Size2u RenderTarget::GetSize() const {
	return size;
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