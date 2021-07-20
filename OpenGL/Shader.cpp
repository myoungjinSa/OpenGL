#include "Shader.h"
#include "Object.h"
#include "Material.h"
#include "Renderer.h"
#include "String/String.h"
#include "Logger.h"
#include "RayCast.h"
#include <fstream>

void MakeWorldMatrix(const Vec3f& position, const Vec3f& look, const Vec3f& right, const Vec3f& up, Matrix<float, 4, 4>& worldMatrix) {
	//Up
	worldMatrix.value[0] = up.x;
	worldMatrix.value[1] = up.y;
	worldMatrix.value[2] = up.z;

	//Right
	worldMatrix.value[4] = right.x;
	worldMatrix.value[5] = right.y;
	worldMatrix.value[6] = right.z;

	//Look
	worldMatrix.value[8] = look.x;
	worldMatrix.value[9] = look.y;
	worldMatrix.value[10] = look.z;

	//Position
	worldMatrix.value[12] = position.x;
	worldMatrix.value[13] = position.y;
	worldMatrix.value[14] = position.z;
	worldMatrix.value[15] = 1.0f;
}

Shader::Shader() 
	:vertexShader(0), fragmentShader(0), shaderProgram(0)
{
	objects.clear();
}

Shader::~Shader() {
	for (auto& obj : objects) {
		delete obj;
	}
	objects.clear();
}

void Shader::Shutdown(Renderer& renderer) {
	for (const auto& obj : objects)
		obj->Shutdown(renderer);
}

void Shader::SetShader(Renderer& renderer) {
	renderer.SetShader(shaderProgram);
}

void Shader::ShutdownShader(Renderer& renderer) {
	renderer.Shutdown(shaderProgram, vertexShader, fragmentShader);
}



size_t Shader::GetObjectCount() const {
	return objects.size();
}

bool Shader::IntersectObjects(const Ray& ray) const {
	double distance = 0.0;
	for (size_t iObj = 0; iObj < objects.size(); iObj++) {
		objects[iObj]->Intersect(ray, distance);
	}

	return true;
}

ColorShader::ColorShader() 
	:Shader()
{

}
ColorShader::~ColorShader() {

}

bool ColorShader::Initialize(Renderer& renderer) {
	bool result;
	result = InitializeShader("color.vs", "color.ps", renderer);

	Object* pObject = new Cube();
	objects.emplace_back(pObject);

	for (const auto& obj : objects) {
		if (!obj->Initialize(renderer)) {
			LogError(L"Could not initialize the model object");
			return false;
		}
	}

	if (!result)
		return false;
	return true;
}

void ColorShader::Shutdown(Renderer& renderer) {
	Shader::Shutdown(renderer);
	ShutdownShader(renderer);
}
void ColorShader::Update(float elapsedTime) {

}
void ColorShader::Render(Renderer& renderer) {
	for (const auto& obj : objects)
		obj->Render(renderer);
}

bool ColorShader::InitializeShader(const char* vsFilename, const char* fsFilename, Renderer& renderer) {
	shaderProgram = renderer.CreateShader();

	if (!renderer.CompileVertexShader(vsFilename, vertexShader))
		return false;

	if (!renderer.CompileFragmentShader(fsFilename, fragmentShader))
		return false;

	String inputPosition("inputPosition");
	String inputColor("inputColor");
	return renderer.BindVertexAttrib(shaderProgram, vertexShader, fragmentShader, 2, inputPosition, inputColor);
}

bool ColorShader::SetShaderParameters(Renderer& renderer, Matrix<float, 4, 4>& worldMatrix, Matrix<float, 4, 4>& viewMatrix, Matrix<float, 4, 4>& projectionMatrix) {
	if (!renderer.SetShaderParameter(shaderProgram, worldMatrix, std::move(String("worldMatrix")))) {
		return false;
	}
	if (!renderer.SetShaderParameter(shaderProgram, viewMatrix, std::move(String("viewMatrix")))) {
		return false;
	}
	if (!renderer.SetShaderParameter(shaderProgram, projectionMatrix, std::move(String("projectionMatrix")))) {
		return false;
	}

	return true;
}

/////////////////////////////Texture Shader//////////////////////////////////////////////////////////////////
PhongShader::PhongShader() 
	: Shader()
{

}

PhongShader::~PhongShader() {

}

bool PhongShader::Initialize(Renderer& renderer) {
	bool result;
	result = InitializeShader("PhongLight.vs", "PhongLight.ps", renderer);
	if (!result)
		return false;

	Object* pObject = new Sphere(2.0f, 36, 36);
	objects.emplace_back(pObject);

	for (const auto& obj : objects) {
		if (!obj->Initialize(renderer)) {
			LogError(L"Could not initialize the model object");
			return false;
		}
	}
	return true;
}

void PhongShader::Update(float elapsedTime) {
	for (const auto& obj : objects) {
		obj->Rotate(0.0f, 2.0f * elapsedTime, 0.0f);
	}
}
void PhongShader::Render(Renderer& renderer, Matrix<float, 4, 4>& viewMatrix, Matrix<float, 4, 4>& projectionMatrix ,Vec3f& lightPosition, Vec3f& cameraPosition) {
	SetShader(renderer);
	for (size_t iObj = 0; iObj < objects.size(); iObj++) {
		SetShaderParameters(renderer, viewMatrix, projectionMatrix, lightPosition, cameraPosition, iObj);
		objects[iObj]->Render(renderer);
	}
}

void PhongShader::Shutdown(Renderer& renderer) {
	Shader::Shutdown(renderer);
	ShutdownShader(renderer);
}

bool PhongShader::InitializeShader(const char* vsFilename, const char* fsFilename, Renderer& renderer) {
	shaderProgram = renderer.CreateShader();

	if (!renderer.CompileVertexShader(vsFilename, vertexShader))
		return false;

	if (!renderer.CompileFragmentShader(fsFilename, fragmentShader))
		return false;

	String inputPosition("inputPosition");
	String inputTexCoord("inputTexCoord");
	String inputNormal("inputNormal");
	return renderer.BindVertexAttrib(shaderProgram, vertexShader, fragmentShader, 3, inputPosition, inputTexCoord, inputNormal);
}

bool PhongShader::SetShaderParameters(Renderer& renderer, Matrix<float, 4, 4>& viewMatrix, Matrix<float, 4, 4>& projectionMatrix, Vec3f& lightPosition, Vec3f& cameraPosition, int objectIndex) {
	if (objects.size() <= objectIndex) {
		LogError(L"오브젝트 인덱스 오류%d", objectIndex);
		assert(0);
		return false;
	}

	Matrix<float, 4, 4> worldMatrix;
	MakeWorldMatrix(objects[objectIndex]->GetPosition(), objects[objectIndex]->GetLook(), objects[objectIndex]->GetRight(), objects[objectIndex]->GetUp(), worldMatrix);
	if (!renderer.SetShaderParameter(shaderProgram, worldMatrix, String("worldMatrix"))) {
		assert(0);
		return false;
	}

	if (!renderer.SetShaderParameter(shaderProgram, viewMatrix, String("viewMatrix"))) {
		assert(0);
		return false;
	}

	if (!renderer.SetShaderParameter(shaderProgram, projectionMatrix, String("projectionMatrix"))) {
		assert(0);
		return false;
	}

	if (!renderer.SetShaderParameter(shaderProgram, lightPosition, String("lightPosition"))) {
		assert(0);
		return false;
	}

	if (!renderer.SetShaderParameter(shaderProgram, cameraPosition, String("worldCameraPosition"))) {
		assert(0);
		return false;
	}


	Vec3f diffuseAlbedo = objects[objectIndex]->material->GetDiffuseAlbedo();
	Vec3f ambientAlbedo = Vec3f(objects[objectIndex]->material->GetAmbientAlbedo().x, objects[objectIndex]->material->GetAmbientAlbedo().y, objects[objectIndex]->material->GetAmbientAlbedo().z);
	Vec3f specular = objects[objectIndex]->material->GetSpecularAlbedo();

	if (!renderer.SetShaderParameter(shaderProgram, diffuseAlbedo, String("diffuseAlbedo"))) {
		assert(0);
		return false;
	}
	if (!renderer.SetShaderParameter(shaderProgram, specular, String("specularAlbedo"))) {
		assert(0);
		return false;
	}
	if (!renderer.SetShaderParameter(shaderProgram, ambientAlbedo, String("ambientAlbedo"))) {
		assert(0);
		return false;
	}

	if (!renderer.SetShaderParameter(shaderProgram, (int)objects[objectIndex]->material->GetTextureUnit(), String("shaderTexture"))) {
		assert(0);
		return false;
	}

	return true;
}

