#include "Shader.h"
#include "Object.h"
#include "Material.h"
#include "Renderer.h"
#include "String/String.h"
#include "Logger.h"
#include "RayCast.h"
#include "BoundingVolume.h"
#include <fstream>


Shader::Shader(Object* pOwner)
	:Component(pOwner), vertexShader(0), fragmentShader(0), shaderProgram(0)
{

}

Shader::~Shader() {

}

void Shader::Shutdown(Renderer& renderer) {
	renderer.Shutdown(shaderProgram, vertexShader, fragmentShader);
}

void Shader::SetShader(Renderer& renderer) {
	renderer.SetShader(shaderProgram);
}


ColorShader::ColorShader(Object* pOwner)
	:Shader(pOwner)
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
}

void ColorShader::Render(Renderer& renderer, const ShaderParameter& shaderParam) {
	SetShader(renderer);
	SetShaderParameters(renderer, shaderParam);
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

bool ColorShader::SetShaderParameters(Renderer& renderer, const ShaderParameter& shaderParam) {
	if (!renderer.SetShaderParameter(shaderProgram, shaderParam.worldMatrix, String("worldMatrix"))) {
		return false;
	}
	if (!renderer.SetShaderParameter(shaderProgram, shaderParam.viewMatrix, String("viewMatrix"))) {
		return false;
	}
	if (!renderer.SetShaderParameter(shaderProgram, shaderParam.projectionMatrix, String("projectionMatrix"))) {
		return false;
	}
	
	renderer.SetShaderParameter(shaderProgram, (int)shaderParam.objNo, String("objNo"));
	renderer.SetShaderParameter(shaderProgram, shaderParam.focusInfo.highlightColor, String("focusInfo.focusingColor"));
	renderer.SetShaderParameter(shaderProgram, (int)shaderParam.focusInfo.focusObjNo, String("focusInfo.focusObjNo"));

	return true;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
TextureShader::TextureShader(Object* pOwner) 
	: Shader(pOwner)
{
	
}

TextureShader::~TextureShader() {

}

bool TextureShader::Initialize(Renderer& renderer) {
	return InitializeShader("Texture.vs", "Texture.ps", renderer);
}

bool TextureShader::InitializeShader(const char* vsFilename, const char* fsFilename,  Renderer& renderer) {
	shaderProgram = renderer.CreateShader();

	if (!renderer.CompileVertexShader(vsFilename, vertexShader))
		return false;

	if (!renderer.CompileFragmentShader(fsFilename, fragmentShader))
		return false;

	String inputPosition("inputPosition");
	String inputColor("inputColor");
	String inputTexCoord("inputTexCoord");

	return renderer.BindVertexAttrib(shaderProgram, vertexShader, fragmentShader, 3, inputPosition, inputColor, inputTexCoord);
}

bool TextureShader::SetShaderParameters(Renderer& renderer, const ShaderParameter& shaderParam) {
	if (!renderer.SetShaderParameter(shaderProgram, shaderParam.worldMatrix, String("worldMatrix"))) {
		return false;
	}
	if (!renderer.SetShaderParameter(shaderProgram, shaderParam.viewMatrix, String("viewMatrix"))) {
		return false;
	}
	if (!renderer.SetShaderParameter(shaderProgram, shaderParam.projectionMatrix, String("projectionMatrix"))) {
		return false;
	}

	int textureUnit = shaderParam.textureUnit;
	if (!renderer.SetShaderParameter(shaderProgram, textureUnit, String("shaderTexture"))) {
		assert(0);
		return false;
	}


	return true;
}

void TextureShader::Render(Renderer& renderer, const ShaderParameter& shaderParam) {
	SetShader(renderer);

	SetShaderParameters(renderer, shaderParam);
}

void TextureShader::Shutdown(Renderer& renderer) {
	Shader::Shutdown(renderer);
}

/////////////////////////////Texture Shader//////////////////////////////////////////////////////////////////
PhongShader::PhongShader(Object* pOwner) 
	: Shader(pOwner)
{

}

PhongShader::~PhongShader() {

}

bool PhongShader::Initialize(Renderer& renderer) {
	return InitializeShader("PhongLight.vs", "PhongLight.ps", renderer);
}


void PhongShader::Render(Renderer& renderer, const ShaderParameter& shaderParameter) {
	SetShader(renderer);
	
	SetShaderParameters(renderer, shaderParameter);
}

void PhongShader::Shutdown(Renderer& renderer) {
	Shader::Shutdown(renderer);
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

bool PhongShader::SetShaderParameters(Renderer& renderer, const ShaderParameter& shaderParam) {

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


	Vec3f diffuse = Vec3f(shaderParam.diffuse.x, shaderParam.diffuse.y, shaderParam.diffuse.z);
	Vec3f ambient = Vec3f(shaderParam.ambient.x, shaderParam.ambient.y, shaderParam.ambient.z);
	Vec3f specular = Vec3f(shaderParam.specular.x, shaderParam.specular.y, shaderParam.specular.z);

	if (!renderer.SetShaderParameter(shaderProgram, diffuse, String("diffuseColor"))) {
		assert(0);
		return false;
	}
	if (!renderer.SetShaderParameter(shaderProgram, specular, String("specularColor"))) {
		assert(0);
		return false;
	}
	if (!renderer.SetShaderParameter(shaderProgram, ambient, String("ambientColor"))) {
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


/////////////////////////////////////////////////////////////////////////////////////////////
//Skybox Shader

SkyboxShader::SkyboxShader(Object* pOwner)
	:Shader(pOwner)
{

}

SkyboxShader::~SkyboxShader() {

}

bool SkyboxShader::Initialize(Renderer& renderer) {
	return InitializeShader("Skybox.vs", "Skybox.ps", renderer);
}

void SkyboxShader::Render(Renderer& renderer, const ShaderParameter& shaderParameter) {
	SetShader(renderer);

	SetShaderParameters(renderer, shaderParameter);
}

void SkyboxShader::Shutdown(Renderer& renderer) {
	Shader::Shutdown(renderer);
}




bool SkyboxShader::InitializeShader(const char* vsFilename, const char* fsFilename, Renderer& renderer) {
	shaderProgram = renderer.CreateShader();

	if (!renderer.CompileVertexShader(vsFilename, vertexShader))
		return false;

	if (!renderer.CompileFragmentShader(fsFilename, fragmentShader))
		return false;

	String inputPosition("inputPosition");
	return renderer.BindVertexAttrib(shaderProgram, vertexShader, fragmentShader, 1, inputPosition);
}

bool SkyboxShader::SetShaderParameters(Renderer& renderer, const ShaderParameter& shaderParam) {
	if (!renderer.SetShaderParameter(shaderProgram, shaderParam.viewMatrix, String("viewMatrix"))) {
		assert(0);
		return false;
	}

	if (!renderer.SetShaderParameter(shaderProgram, shaderParam.projectionMatrix, String("projectionMatrix"))) {
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
