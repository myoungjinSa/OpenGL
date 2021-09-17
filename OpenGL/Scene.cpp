#include "Common.h"
#include "Shader.h"
#include "Camera.h"
#include "Scene.h"
#include "Renderer.h"
#include "RayCast.h"
#include "Logger.h"
#include "Material.h"



Scene::Scene() 
	:pCamera(nullptr),
	 projectionMatrix(),
	 Gizmo()
{

}

Scene::~Scene() {

	for (auto& obj : objects) {
		delete obj;
	}
	objects.clear();

	if (pCamera) {
		delete pCamera;
		pCamera = nullptr;
	}
}

bool Scene::BuildObject(Renderer& renderer) {	
	phongLight.SetPosition(0.0f, 0.0f, -10.0f);
	phongLight.SetDirection(Vec3f::FORWARD);

	DefaultShader = std::make_shared<PhongShader>(this);
	if (!DefaultShader)
		return false;

	if (!DefaultShader->Initialize(renderer)) {
		LogError(L"Could not initialize the Default Shader\n");
		return false;
	}

	GameObject* pObject = new Sphere(2.0f, 16, 16);//new Cylinder(Vec3f::UP, Vec3f::RIGHT, Vec3f::FORWARD, 32);
	objects.emplace_back(pObject);

	for (const auto& obj : objects) {
		if (!obj->Initialize(renderer)) {
			LogError(L"Could not initialize the model object\n");
			return false;
		}

	}

	pCamera = new Camera();
	if (!pCamera) {
		assert(0);
		return false;
	}

	pCamera->Initialize(renderer);
	pCamera->SetPosition(0.0f, 0.0f, -10.0f);
	pCamera->SetViewport(Rect2f(0.0f, 0.0f, renderer.GetRenderTargetWidth(), renderer.GetRenderTargetHeight()));
	
	Gizmo.Initialize(renderer);

	return true;
}

void Scene::Update(const float& elapsedTime) {
	assert(pCamera);
	pCamera->Update(elapsedTime);


	for (const auto& obj : objects) {
		obj->Rotate(MathUtils::DegreesToRadians(0.0f), MathUtils::DegreesToRadians(1.0f), MathUtils::DegreesToRadians(0.0f));
		//obj->Move(obj->GetLook(), 1.0f, elapsedTime);
	}

	Gizmo.Update(*pCamera, elapsedTime);
}

bool Scene::Render(Renderer& renderer) {
	renderer.BeginRender();
	pCamera->BuildPerspectiveFovLHMatrix(projectionMatrix, pCamera->GetViewport(), SCREEN_NEAR, SCREEN_DEPTH);

	Matrix<float, 4, 4> worldMatrix;
	for (size_t iObj = 0; iObj < objects.size(); iObj++) {
		ShaderParameter shaderParmaeter;
		FillShaderParameter(*objects[iObj], shaderParmaeter);
		DefaultShader->Render(renderer, shaderParmaeter);
		
		objects[iObj]->Render(renderer, GetViewMatrix(), GetProjectionMatrix());
	}
	Gizmo.Render(renderer, pCamera, *this);
	renderer.EndRender();
	return true;
}

void Scene::Shutdown(Renderer& renderer) {
	if(DefaultShader)
		DefaultShader->Shutdown(renderer);

	for (const auto& obj : objects)
		obj->Shutdown(renderer);

	if (pCamera)
		pCamera->Shutdown(renderer);
}

Matrix<float, 4, 4> Scene::GetViewMatrix() const {
	Matrix<float, 4, 4> viewMatrix;
	if (pCamera)
		pCamera->GetViewMatrix(viewMatrix);

	return viewMatrix;
}
Matrix<float, 4, 4> Scene::GetProjectionMatrix() const {
	return projectionMatrix;
}

GameObject* Scene::GetGameObject(uint32_t idx) const{
	if (objects.size() <= idx)
		return nullptr;

	return objects[idx];
}

Vec3f Scene::GetCameraPosition() const {
	assert(pCamera);
	return pCamera->GetPosition();
}
size_t Scene::GetObjectCount() const {
	return objects.size();
}
void Scene::Picking(int x, int y, int screenWidth, int screenHeight) {
	RayCast rayCast(*this);

	GameObject* pHitObject = rayCast.HitTest(*this, x, y, screenWidth, screenHeight);
	
	if (pHitObject) {
		if (Gizmo.IsAlreadyAttached()) {
			const GameObject& pAttachedObjects = Gizmo.GetAttachedObjects(0);
			if (GameObject* pGizmo = rayCast.HitTest(Gizmo, x, y, screenWidth, screenHeight)) {
				
				//pGizmo->
			}

		}else {
			Gizmo.Attach(*pHitObject);
		}
	}
}

void Scene::FillShaderParameter(const GameObject& gameObject, ShaderParameter& shaderParam) {
	Matrix<float, 4, 4> worldMatrix  = Matrix<float, 4 , 4>::Identity();
	MakeWorldMatrix(gameObject.GetPosition(), gameObject.GetLook(), gameObject.GetRight(), gameObject.GetUp(), worldMatrix);
	shaderParam.worldMatrix = worldMatrix;
	shaderParam.viewMatrix = GetViewMatrix();
	shaderParam.projectionMatrix = GetProjectionMatrix();

	shaderParam.lightPosition = phongLight.GetPosition();
	shaderParam.diffuseAlbedo = gameObject.material->GetDiffuseAlbedo();
	shaderParam.ambientAlbedo = gameObject.material->GetAmbientAlbedo();
	shaderParam.specularAlbedo = gameObject.material->GetSpecularAlbedo();

	shaderParam.cameraPosition = pCamera->GetPosition();

	shaderParam.textureUnit = gameObject.material->GetTextureUnit(Material::TextureType::TEXTURE_ALBEDO);
}

bool Scene::IntersectObjects(const Ray& ray) const {
	double distance = 0.0;
	for (size_t iObj = 0; iObj < objects.size(); iObj++) {
		if (objects[iObj]->Intersect(ray, distance))
			return true;
	}
	return false;
}


