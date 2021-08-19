#include "Shader.h"
#include "Object.h"
#include "Material.h"
#include "Renderer.h"
#include "String/String.h"
#include "Logger.h"
#include "RayCast.h"
#include "BoundingVolume.h"
#include <fstream>


Shader::Shader() 
	:vertexShader(0), fragmentShader(0), shaderProgram(0)
{

}

Shader::~Shader() {

}

void Shader::Shutdown(Renderer& renderer) {

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

bool ColorShader::Initialize(Renderer& renderer) {
	if (!InitializeShader("color.vs", "color.ps", renderer))
		return false;

	return true;
}

void ColorShader::Shutdown(Renderer& renderer) {
	Shader::Shutdown(renderer);
	ShutdownShader(renderer);
}
void ColorShader::Update(float elapsedTime) {

}
void ColorShader::Render(Renderer& renderer, ShaderParameter& shaderParam) {
	SetShader(renderer);
	renderer.SetDrawMode(Renderer::DrawMode::TRIANGLES);
	SetShaderParameters(renderer, shaderParam);
	/*for (size_t iObj = 0; iObj < objects.size(); iObj++) {
		SetShaderParameters(renderer, viewMatrix, projectionMatrix, iObj);
		objects[iObj]->Render(renderer);
	}*/
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

bool ColorShader::SetShaderParameters(Renderer& renderer, ShaderParameter& shaderParam) {
	if (!renderer.SetShaderParameter(shaderProgram, shaderParam.worldMatrix, std::move(String("worldMatrix")))) {
		return false;
	}
	if (!renderer.SetShaderParameter(shaderProgram, shaderParam.viewMatrix, std::move(String("viewMatrix")))) {
		return false;
	}
	if (!renderer.SetShaderParameter(shaderProgram, shaderParam.projectionMatrix, std::move(String("projectionMatrix")))) {
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

	return true;
}

void PhongShader::Update(float elapsedTime) {

}
void PhongShader::Render(Renderer& renderer, ShaderParameter& shaderParameter) {
	SetShader(renderer);
	
	SetShaderParameters(renderer, shaderParameter);
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

bool PhongShader::SetShaderParameters(Renderer& renderer, ShaderParameter& shaderParam) {

	if (!renderer.SetShaderParameter(shaderProgram, shaderParam.worldMatrix, String("worldMatrix"))) {
		assert(0);
		return false;
	}

	if (!renderer.SetShaderParameter(shaderProgram, shaderParam.viewMatrix, String("viewMatrix"))) {
		assert(0);
		return false;
	}

	if (!renderer.SetShaderParameter(shaderProgram, shaderParam.projectionMatrix, String("projectionMatrix"))) {
		assert(0);
		return false;
	}

	if (!renderer.SetShaderParameter(shaderProgram, shaderParam.lightPosition, String("lightPosition"))) {
		assert(0);
		return false;
	}

	if (!renderer.SetShaderParameter(shaderProgram, shaderParam.cameraPosition, String("worldCameraPosition"))) {
		assert(0);
		return false;
	}


	Vec3f diffuseAlbedo = Vec3f(shaderParam.diffuseAlbedo.x, shaderParam.diffuseAlbedo.y, shaderParam.diffuseAlbedo.z);
	Vec3f ambientAlbedo = Vec3f(shaderParam.ambientAlbedo.x, shaderParam.ambientAlbedo.y, shaderParam.ambientAlbedo.z);
	Vec3f specular = Vec3f(shaderParam.specularAlbedo.x, shaderParam.specularAlbedo.y, shaderParam.specularAlbedo.z);

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

	int textureUnit = shaderParam.textureUnit;
	if (!renderer.SetShaderParameter(shaderProgram, textureUnit, String("shaderTexture"))) {
		assert(0);
		return false;
	}

	return true;
}

