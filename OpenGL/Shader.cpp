#include "Shader.h"
#include "Object.h"
#include "Material.h"
#include "Renderer.h"
#include "String/String.h"
#include "Logger.h"
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

ColorShader::ColorShader() 
	:Shader()
{

}
ColorShader::~ColorShader() {

}

bool ColorShader::Initialize(Renderer& renderer, HWND hWnd) {
	bool result;
	result = InitializeShader("color.vs", "color.ps", renderer, hWnd);

	Object* pObject = new Cube();
	objects.emplace_back(pObject);

	for (const auto& obj : objects) {
		if (!obj->Initialize(renderer)) {
			MessageBox(hWnd, L"Could not initialize the model object", L"Error", MB_OK);
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

void ColorShader::Render(Renderer& renderer) {
	for (const auto& obj : objects)
		obj->Render(renderer);
}

bool ColorShader::InitializeShader(const char* vsFilename, const char* fsFilename, Renderer& renderer, HWND hWnd) {
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

bool PhongShader::Initialize(Renderer& renderer, HWND hWnd) {
	bool result;
	result = InitializeShader("PhongLight.vs", "PhongLight.ps", renderer, hWnd);
	if (!result)
		return false;

	Object* pObject = new Cube();
	objects.emplace_back(pObject);

	for (const auto& obj : objects) {
		if (!obj->Initialize(renderer)) {
			MessageBox(hWnd, L"Could not initialize the model object", L"Error", MB_OK);
			return false;
		}
	}
	return true;
}

void PhongShader::Render(Renderer& renderer, Matrix<float, 4, 4>& viewMatrix, Matrix<float, 4, 4>& projectionMatrix ,Vec3f& lightPosition, Vec3f& lightDirection) {
	SetShader(renderer);
	
	for (size_t iObj = 0; iObj < objects.size(); iObj++) {
		SetShaderParameters(renderer, viewMatrix, projectionMatrix, lightDirection, iObj);
		objects[iObj]->Render(renderer);
	}
}

void PhongShader::Shutdown(Renderer& renderer) {
	Shader::Shutdown(renderer);
	ShutdownShader(renderer);
}

bool PhongShader::InitializeShader(const char* vsFilename, const char* fsFilename, Renderer& renderer, HWND hWnd) {
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

bool PhongShader::SetShaderParameters(Renderer& renderer, Matrix<float, 4, 4>& viewMatrix, Matrix<float, 4, 4>& projectionMatrix, Vec3f& lightDirection, int objectIndex) {
	if (objects.size() <= objectIndex) {
		LogError(L"오브젝트 인덱스 오류%d", objectIndex);
		assert(0);
		return false;
	}

	Matrix<float, 4, 4> worldMatrix;
	MakeWorldMatrix(objects[objectIndex]->GetPosition(), objects[objectIndex]->GetLook(), objects[objectIndex]->GetRight(), objects[objectIndex]->GetUp(), worldMatrix);
	if (!renderer.SetShaderParameter(shaderProgram, worldMatrix, std::move(String("worldMatrix")))) {
		return false;
	}

	if (!renderer.SetShaderParameter(shaderProgram, viewMatrix, std::move(String("viewMatrix")))) {
		return false;
	}

	if (!renderer.SetShaderParameter(shaderProgram, projectionMatrix, std::move(String("projectionMatrix")))) {
		return false;
	}

	if (!renderer.SetShaderParameter(shaderProgram, lightDirection, std::move(String("lightDirection")))) {
		return false;
	}
	
	Vec4f diffuseAlbedo = Vec4f(objects[objectIndex]->material->GetDiffuseAlbedo(), 1.0f);
	Vec4f ambientAlbedo = objects[objectIndex]->material->GetAmbientAlbedo();
	Vec4f specular = Vec4f(objects[objectIndex]->material->GetSpecularAlbedo(), 1.0f);
	
	if (!renderer.SetShaderParameter(shaderProgram, diffuseAlbedo, std::move(String("diffuseAlbedo")))) {
		return false;
	}
	if (!renderer.SetShaderParameter(shaderProgram, ambientAlbedo, std::move(String("ambientAlbedo")))) {
		return false;
	}
	if (!renderer.SetShaderParameter(shaderProgram, specular, std::move(String("specularAlbedo")))) {
		return false;
	}
	if (!renderer.SetShaderParameter(shaderProgram, (int)objects[objectIndex]->material->GetTextureUnit(), std::move(String("shaderTexture")))) {
		return false;
	}
	return true;
}

