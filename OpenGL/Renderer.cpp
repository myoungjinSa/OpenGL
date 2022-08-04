#include "Renderer.h"
#include "Camera.h"
#include "Scene.h"
#include "Shader.h"
#include "OpenGL.h"
#include "Object.h"
#include "Texture.h"
#include "String/String.h"
#include "Mesh.h"
#include <fstream>
#include <stdarg.h>

Renderer::Renderer() 
	:drawMode(DrawMode::TRIANGLES)
{
}

Renderer::~Renderer() {

}


void Renderer::Shutdown(unsigned int shaderProgram, unsigned int vertexShader, unsigned int fragmentShader) {
	TextureLoader::Release();
}



bool Renderer::Initialize(GraphicDevice* pGraphics) {
	screenSize = Size2u(pGraphics->GetScreenWidth(), pGraphics->GetScreenHeight());
	return false;
}

char* Renderer::LoadShaderSourceFile(const char* filename) {
	std::ifstream fin;

	//Open the shader source file.
	fin.open(filename);
	if (fin.fail())
		return 0;

	int fileSize = 0;

	//Read the first element of the file
	char input;
	fin.get(input);

	//Count the number of elements in the text file
	while (!fin.eof()) {
		fileSize++;
		fin.get(input);
	}
	//Close the file for now
	fin.close();

	//Initialize the buffer to read the shader source file into.
	char* buffer = new char[fileSize + 1];
	if (!buffer) {
		return 0;
	}

	fin.open(filename);
	fin.read(buffer, fileSize);
	fin.close();

	buffer[fileSize] = '\0';

	return buffer;
}

void Renderer::OutputLinkErrorMessage(GraphicDevice& device, unsigned int programId) {
	char* pInfoLog = device.GetLinkerLog(programId);
	if (!pInfoLog)
		return;

	std::ofstream fout;
	fout.open("linker-error.txt");
	int logSize = strlen(pInfoLog);
	for (size_t iLog = 0; iLog < logSize; iLog++)
		fout << pInfoLog[iLog];

	fout.close();
	delete[] pInfoLog;

	MessageBox(hWnd, L"Error compiling linker. Check linker-error.txt for message", L"Linker Error", MB_OK);
}

void Renderer::OutputShaderErrorMessage(GraphicDevice& device, unsigned int shaderId, char* shaderFilename) {

	char* pInfoLog = device.GetShaderLog(shaderId);
	if (!pInfoLog)
		return;

	std::ofstream fout;
	//Open a file to write the error message to.
	fout.open("shader-error.txt");
	int logSize = strlen(pInfoLog);
	for (size_t iLog = 0; iLog < logSize; iLog++) {
		fout << pInfoLog[iLog];
	}

	fout.close();
	delete[] pInfoLog;

	//Convert the shader filename to a widechar string
	size_t convertedChars = 0;
	wchar_t newString[128];
	size_t error = mbstowcs_s(&convertedChars, newString, 128, shaderFilename, 128);
	if (error != 0)
		return;

	MessageBox(hWnd, L"Error compile shader. Check shader-error.txt for message.", newString, MB_OK);
}

void Renderer::SetDrawMode(Renderer::DrawMode _drawMode) {
	drawMode = _drawMode;
}

void Renderer::SetRenderMode(Renderer::RenderMode _renderMode) {
	renderMode = _renderMode;
	switch (renderMode) {
	case RenderMode::SOLID:
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	case RenderMode::WIREFRAME:
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	}
}

OpenGLRenderer::~OpenGLRenderer() {

}

bool OpenGLRenderer::Initialize(GraphicDevice* pGraphicDevice) {
	Renderer::Initialize(pGraphicDevice);
	//Store a pointer to ther OpenGL class object.
	pDevice = dynamic_cast<OpenGL*>(pGraphicDevice);
	if (!pDevice)
		return false;

	return true;
}
void OpenGLRenderer::Shutdown(unsigned int shaderProgram, unsigned int vertexShader, unsigned int fragmentShader) {
	Renderer::Shutdown(shaderProgram, vertexShader, fragmentShader);
	
	pDevice->glDetachShader(shaderProgram, vertexShader);
	pDevice->glDetachShader(shaderProgram, fragmentShader);

	pDevice->glDeleteShader(vertexShader);
	pDevice->glDeleteShader(fragmentShader);

	pDevice->glDeleteProgram(shaderProgram);
}

unsigned int OpenGLRenderer::CreateShader() {
	return pDevice->glCreateProgram();
}

bool OpenGLRenderer::CreateRenderTarget(RenderTarget& renderTarget, const Size2u& screenSize, bool downSamplingTarget) {
	if (screenSize.Empty())
		return false;

	renderTarget.SetSize(screenSize);
	int iPreviousFrameBuffer = 0;

	if (renderTarget.IsNull()) {
		pDevice->glGenFramebuffersEXT(1, &(renderTarget.iFrameBuffer));
		if (pDevice->glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			return false;
		//OpenGL::CheckError();
	}
	pDevice->glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, renderTarget.iFrameBuffer);
	OpenGL::CheckError();

	renderTarget.colorTexture = std::make_shared<Texture>();
	if (!CreateTexture(*renderTarget.colorTexture, screenSize, NULL))
		return false;

	SetSampleMode(false, GL_LINEAR, GL_CLAMP_TO_EDGE);

	pDevice->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTarget.colorTexture.get()->textureID, 0);
	
	renderTarget.depthStencilTexture = std::make_shared<Texture>();
	if (!CreateDepthStencilTexture(*renderTarget.depthStencilTexture, screenSize))
		return false;

	SetSampleMode(false, GL_LINEAR, GL_CLAMP_TO_EDGE);
	pDevice->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, renderTarget.depthStencilTexture.get()->textureID, 0);

	if (pDevice->glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		assert(0);
		return false;
	}

	pDevice->glBindFramebufferEXT(GL_FRAMEBUFFER, 0);

	if (!renderTarget.Create(*this))
		return false;

	return true;
}

void OpenGLRenderer::DestroyRenderTarget(RenderTarget& renderTarget) {
	glDeleteTextures(1, &(renderTarget.colorTexture->textureID));
	glDeleteTextures(1, &(renderTarget.depthStencilTexture->textureID));
}

bool OpenGLRenderer::CreateTexture(Texture& texture, const Size2u& size, void* pImage) {
	if (size.Empty())
		return false;

	if (texture.IsNull()) {
		AllocateTextures(texture.textureID, 1);
	}

	BindTexture(texture.textureID);
	OpenGL::CheckError();

	SetImage(GL_TEXTURE_2D, pImage, size.width, size.height, GL_RGBA, GL_BGRA, GL_UNSIGNED_BYTE);
	
	return true;
}

bool OpenGLRenderer::CreateDepthStencilTexture(Texture& texture, const Size2u& size) {
	if (size.Empty())
		return false;

	if (texture.IsNull())
		AllocateTextures(texture.textureID, 1);

	BindTexture(texture.textureID);
	OpenGL::CheckError();
	SetImage(GL_TEXTURE_2D, NULL, size.width, size.height, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8);
	//pDevice->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, texture.textureID, NULL);
	return true;
}

void OpenGLRenderer::DeleteTexture(Texture& texture) {
	glDeleteTextures(1, &texture.textureID);
}
void OpenGLRenderer::SetShader(unsigned int shaderPrgram) {
	pDevice->glUseProgram(shaderPrgram);
}

bool OpenGLRenderer::CompileVertexShader(const char* vsFilename, unsigned int& vertexShader) {
	const char* vertexShaderBuffer;
	vertexShaderBuffer = LoadShaderSourceFile(vsFilename);
	if (!vertexShaderBuffer)
	{
		return false;
	}
	vertexShader = pDevice->glCreateShader(GL_VERTEX_SHADER);
	pDevice->glShaderSource(vertexShader, 1, &vertexShaderBuffer, NULL);
	delete vertexShaderBuffer;
	vertexShaderBuffer = nullptr;

	int status = 0;
	pDevice->glCompileShader(vertexShader);
	pDevice->glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
	if (status != 1) {
		// If it did not compile then write the syntax error message out to a text file for review.
		OutputShaderErrorMessage(*pDevice, vertexShader, const_cast<char*>(vsFilename));
		return false;
	}
	return true;
}

bool OpenGLRenderer::CompileFragmentShader(const char* fsFilename, unsigned int& fragmentShader) {
	const char* fragmentShaderBuffer;
	fragmentShaderBuffer = LoadShaderSourceFile(fsFilename);
	if (!fragmentShaderBuffer)
	{
		return false;
	}
	fragmentShader = pDevice->glCreateShader(GL_FRAGMENT_SHADER);
	pDevice->glShaderSource(fragmentShader, 1, &fragmentShaderBuffer, NULL);
	delete fragmentShaderBuffer;
	fragmentShaderBuffer = nullptr;

	int status = 0;
	pDevice->glCompileShader(fragmentShader);
	pDevice->glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
	if (status != 1) {
		// If it did not compile then write the syntax error message out to a text file for review.
		OutputShaderErrorMessage(*pDevice, fragmentShader, const_cast<char*>(fsFilename));
		return false;
	}
	return true;
}

bool OpenGLRenderer::CompileGeometryShader(const char* gsFilename, unsigned int& geometryShader) {
	const char* geometryShaderBuffer;
	geometryShaderBuffer = LoadShaderSourceFile(gsFilename);
	if (!geometryShaderBuffer)
		return false;

	geometryShader = pDevice->glCreateShader(GL_GEOMETRY_SHADER);
	pDevice->glShaderSource(geometryShader, 1, &geometryShaderBuffer, NULL);
	delete geometryShaderBuffer;
	geometryShaderBuffer = nullptr;

	int status = 0;
	pDevice->glCompileShader(geometryShader);
	pDevice->glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &status);
	if (status != 1) {
		OutputShaderErrorMessage(*pDevice, geometryShader, const_cast<char*>(gsFilename));
		return false;
	}

	return true;
}


bool OpenGLRenderer::BindVertexAttrib(unsigned int shaderProgram, unsigned int vertexShader, unsigned int fragmentShader, int vertexArgs, ...) {
	pDevice->glAttachShader(shaderProgram, vertexShader);
	pDevice->glAttachShader(shaderProgram, fragmentShader);

	va_list ap;
	va_start(ap, vertexArgs);
	String attrib;
	for (size_t iAttrib = 0; iAttrib < vertexArgs; iAttrib++) {
		attrib = va_arg(ap, String);

		pDevice->glBindAttribLocation(shaderProgram, iAttrib, attrib.c_str());
	}
	pDevice->glLinkProgram(shaderProgram);

	GLint status;
	pDevice->glGetProgramiv(shaderProgram, GL_LINK_STATUS, &status);
	if (status != GL_TRUE) {
		OutputLinkErrorMessage(*pDevice, shaderProgram);
		return false;
	}
	return true;
}

bool OpenGLRenderer::BindVertexAttrib(unsigned int shaderProgram, unsigned int vertexShader, unsigned int geometryShader, unsigned int fragmentShader, int vertexArgs, ...) {
	pDevice->glAttachShader(shaderProgram, vertexShader);
	pDevice->glAttachShader(shaderProgram, geometryShader);
	pDevice->glAttachShader(shaderProgram, fragmentShader);
	
	va_list ap;
	va_start(ap, vertexArgs);
	String attrib;
	for (size_t iAttrib = 0; iAttrib < vertexArgs; iAttrib++) {
		attrib = va_arg(ap, String);

		pDevice->glBindAttribLocation(shaderProgram, iAttrib, attrib.c_str());
	}
	pDevice->glLinkProgram(shaderProgram);

	GLint status;
	pDevice->glGetProgramiv(shaderProgram, GL_LINK_STATUS, &status);
	if (status != GL_TRUE) {
		OutputLinkErrorMessage(*pDevice, shaderProgram);
		return false;
	}
	return true;
}

void OpenGLRenderer::SetViewport(const Viewporti& viewport) {
	Point2i leftTop = viewport.GetLeftTop();
	glViewport(leftTop.x, leftTop.y, viewport.GetWidth(), viewport.GetHeight());
}

void OpenGLRenderer::SetWindingOrder(Renderer::WindingOrder order) {
	switch (order) {
	case WindingOrder::CW:
		glFrontFace(GL_CW);
		break;
	case WindingOrder::CCW:
		glFrontFace(GL_CCW);
		break;
	}
}

void OpenGLRenderer::SetCullFace(eFace _face) {
	int face = GetFace(_face);
	glCullFace(face);
}

void OpenGLRenderer::EnableDepthTest(bool bEnable) {
	bEnable ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
}

bool OpenGLRenderer::SetShaderParameter(unsigned int shaderProgram, Matrix<float, 4, 4>& matrix, String variableName) {
	_ASSERT(pDevice);
	unsigned int location = pDevice->glGetUniformLocation(shaderProgram, variableName.c_str());
	if (location == -1)
		return false;

	pDevice->glUniformMatrix4fv(location, 1, false, (float*)&matrix.value);
	return true;
}

bool OpenGLRenderer::SetShaderParameter(unsigned int shaderProgram, const Matrix<float, 4, 4>& matrix, String variableName) {
	_ASSERT(pDevice);
	unsigned int location = pDevice->glGetUniformLocation(shaderProgram, variableName.c_str());
	if (location == -1)
		return false;

	pDevice->glUniformMatrix4fv(location, 1, false, (float*)&matrix.value);
	return true;
}

bool OpenGLRenderer::SetShaderParameter(unsigned int shaderProgram, const Matrix<float, 3, 3>& matrix, String variableName) {
	_ASSERT(pDevice);
	unsigned int location = pDevice->glGetUniformLocation(shaderProgram, variableName.c_str());
	if (location == -1)
		return false;

	pDevice->glUniformMatrix3fv(location, 1, false, (float*)&matrix.value);
	return true;
}

bool OpenGLRenderer::SetShaderParameter(unsigned int shaderProgram, Matrix<float, 3, 3>& matrix, String variableName) {
	_ASSERT(pDevice);
	unsigned int location = pDevice->glGetUniformLocation(shaderProgram, variableName.c_str());
	if (location == -1)
		return false;

	pDevice->glUniformMatrix3fv(location, 1, false, (float*)&matrix.value);
	return true;
}

bool OpenGLRenderer::SetShaderParameter(unsigned int shaderProgram, const Vec3f& vec3, String variableName) {
	_ASSERT(pDevice);
	unsigned int location = pDevice->glGetUniformLocation(shaderProgram, variableName.c_str());
	if (location == -1)
		return false;

	pDevice->glUniform3fv(location, 1, (float*)const_cast<Vec3f&>(vec3).ConvertToValue());
	return true;
}

bool OpenGLRenderer::SetShaderParameter(unsigned int shaderProgram, Vec3f& vec3, String variableName) {
	_ASSERT(pDevice);
	unsigned int location = pDevice->glGetUniformLocation(shaderProgram, variableName.c_str());
	if (location == -1)
		return false;

	pDevice->glUniform3fv(location, 1, (float*)vec3.ConvertToValue());
	return true;
}

bool OpenGLRenderer::SetShaderParameter(unsigned int shaderProgram, const Vec4f& vec4, String variableName) {
	_ASSERT(pDevice);
	unsigned int location = pDevice->glGetUniformLocation(shaderProgram, variableName.c_str());
	if (location == -1)
		return false;

	pDevice->glUniform4fv(location, 1, (float*)const_cast<Vec4f&>(vec4).ConvertToValue());
	return true;
}

bool OpenGLRenderer::SetShaderParameter(unsigned int shaderProgram, Vec4f& vec4, String variableName) {
	_ASSERT(pDevice);
	unsigned int location = pDevice->glGetUniformLocation(shaderProgram, variableName.c_str());
	if (location == -1)
		return false;

	pDevice->glUniform4fv(location, 1, (float*)vec4.ConvertToValue());
	return true;
}

bool OpenGLRenderer::SetShaderParameter(unsigned int shaderProgram, int integer, String variableName) {
	_ASSERT(pDevice);
	unsigned int location = pDevice->glGetUniformLocation(shaderProgram, variableName.c_str());
	if (location == -1)
		return false;

	pDevice->glUniform1i(location, integer);
	return true;
}

bool OpenGLRenderer::SetShaderParameter(unsigned int shaderProgram, float value, String variableName) {
	_ASSERT(pDevice);
	unsigned int location = pDevice->glGetUniformLocation(shaderProgram, variableName.c_str());
	if (location == -1)
		return false;

	pDevice->glUniform1f(location, value);
	return true;
}

bool OpenGLRenderer::AllocateVertexBuffer(unsigned int& vertexArrayId, unsigned int& vertexBufferId, void* vertexData, VertexBufferBindCallback* pBindFunction, unsigned int numVertices, unsigned int sizeofVertex) {
	pDevice->glGenVertexArrays(1, &vertexArrayId);
	pDevice->glBindVertexArray(vertexArrayId);
	pDevice->glGenBuffers(1, &vertexBufferId);


	pBindFunction(*pDevice, vertexData, vertexBufferId, numVertices, sizeofVertex);
	return true;
}


bool OpenGLRenderer::AllocateIndexBuffer(unsigned int& indexBufferId, size_t indexCount, unsigned int* indexData) {
	if (indexCount == 0)
		return false;

	pDevice->glGenBuffers(1, &indexBufferId);

	pDevice->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
	pDevice->glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned int), indexData, GL_STATIC_DRAW);
	return true;
}

bool OpenGLRenderer::AllocateFrameBuffer(unsigned int& colorBuffer, unsigned int& depthBuffer, unsigned int& fbo) {
	pDevice->glGenFramebuffersEXT(1, &fbo);
	pDevice->glBindFramebufferEXT(GL_FRAMEBUFFER, fbo);

	return true;
}

bool OpenGLRenderer::AllocateTextures(unsigned int& textureId, unsigned int textureCount) {
	//Generate an ID for the texture.
	glGenTextures(textureCount, &textureId);

	OpenGL::CheckError();
	return true;
}


void OpenGLRenderer::SetSampleMode(bool bCubemap, int filterMode, int wrappingMode) {
	if (bCubemap) {
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, wrappingMode);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, wrappingMode);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, wrappingMode);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, filterMode);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, filterMode);
	}
	else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterMode);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrappingMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrappingMode);
	}
}


void OpenGLRenderer::BindTexture(unsigned int textureId) {
	//glEnable(GL_TEXTURE_2D);
	////Set the unique texture unit in which to store the data
	pDevice->glActiveTexture(GL_TEXTURE0 + textureId);

	glBindTexture(GL_TEXTURE_2D, textureId);
}

void OpenGLRenderer::BindCubemapTexture(unsigned int textureId) {
	pDevice->glActiveTexture(GL_TEXTURE0 + textureId);

	glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);
}

void OpenGLRenderer::SetImage(unsigned int target, void* pImage, unsigned int width, unsigned int height, unsigned int textureFormat, unsigned int realFormat, unsigned int dataType) {
	glTexImage2D(target, 0, textureFormat, width, height, 0, realFormat, dataType, pImage);
}

void OpenGLRenderer::DisableVertexAttribArray(size_t vertexAttribCount) {
	for (size_t iVertexAttrib = 0; iVertexAttrib < vertexAttribCount; iVertexAttrib++) {
		pDevice->glDisableVertexAttribArray(iVertexAttrib);
	}
}


void OpenGLRenderer::ReleaseVertexArray(unsigned int& vertexArrayId, size_t arrayCount) {
	pDevice->glBindVertexArray(0);
	pDevice->glDeleteVertexArrays(arrayCount, &vertexArrayId);
}
void OpenGLRenderer::ReleaseVertexBuffers(unsigned int& vertexBufferId, size_t bufferCount) {
	pDevice->glBindBuffer(GL_ARRAY_BUFFER, 0);
	pDevice->glDeleteBuffers(bufferCount, &vertexBufferId);
}
void OpenGLRenderer::ReleaseIndexBuffers(unsigned int& indexBufferId, size_t indexCount) {
	pDevice->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	pDevice->glDeleteBuffers(indexCount, &indexBufferId);
}

void OpenGLRenderer::DrawVertexBuffer(unsigned int vertexArrayId, size_t startPos, size_t vertexCount) {
	pDevice->glBindVertexArray(vertexArrayId);

	switch (drawMode) {
	case DrawMode::TRIANGLE_STRIP:
		glDrawArrays(GL_TRIANGLE_STRIP, startPos, vertexCount);
		break;
	case DrawMode::TRIANGLES:
		glDrawArrays(GL_TRIANGLES, startPos, vertexCount);
		break;
	case DrawMode::LINES:
		glDrawArrays(GL_LINES, startPos, vertexCount);
		break;
	}
}

void OpenGLRenderer::DrawIndexBuffer(unsigned int vertexArrayId, size_t indexCount) {
	pDevice->glBindVertexArray(vertexArrayId);

	switch (drawMode) {
	case DrawMode::TRIANGLE_STRIP:
		glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
		break;
	case DrawMode::TRIANGLES:
		glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
		break;
	case DrawMode::LINES:
		glDrawElements(GL_LINES, indexCount, GL_UNSIGNED_INT, 0);
		break;
	}
}

void OpenGLRenderer::BindRenderTarget(RenderTarget& renderTarget) {
	pDevice->glBindFramebufferEXT(GL_FRAMEBUFFER, renderTarget.iFrameBuffer);

	EnableDepthTest(true);
	
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClearDepth(1.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
}
void OpenGLRenderer::UnbindRenderTarget(RenderTarget& renderTarget) {
	pDevice->glBindFramebufferEXT(GL_FRAMEBUFFER, 0);
}

int OpenGLRenderer::GetFace(eFace _face)const {
	int face = GL_BACK;
	
	switch (_face) {
	case eFace::Back:    face = GL_BACK; break;
	case eFace::Front:   face = GL_FRONT; break;
	case eFace::FrontAndBack: face = GL_FRONT_AND_BACK; break;
	}

	return face;
}
int OpenGLRenderer::GetDepthCompare(eCompare _depthCmp) const {
	int depthCmp = -1;
	switch (_depthCmp) {
	case eCompare::ALWAYS:	depthCmp = GL_ALWAYS;		break;
	case eCompare::NEVER:	depthCmp = GL_NEVER;			break;
	case eCompare::LESS:	depthCmp = GL_LESS;			break;
	case eCompare::LEQUAL:	depthCmp = GL_LEQUAL;		break;
	case eCompare::EQUAL:	depthCmp = GL_EQUAL;			break;
	case eCompare::NOTEQUAL: depthCmp = GL_NOTEQUAL;		break;
	case eCompare::GREATER:	depthCmp = GL_GREATER;		break;
	case eCompare::GEQUAL:	depthCmp = GL_GEQUAL;		break;
	default:
		break;
	}
	return depthCmp;
}

int OpenGLRenderer::GetStencilOp(StencilOp _stencilOp) const {
	int stencilOp = -1;
	switch (_stencilOp) {
	case StencilOp::KEEP:			stencilOp = GL_KEEP;		break;
	case StencilOp::ZERO:			stencilOp = GL_ZERO;		break;
	case StencilOp::REPLACE:		stencilOp = GL_REPLACE;		break;
	case StencilOp::INCREMENT:		stencilOp = GL_INCR;		break;
	case StencilOp::DECREMENT:		stencilOp = GL_DECR;		break;
	case StencilOp::INVERT:			stencilOp = GL_INVERT;		break;
	case StencilOp::INCREMENT_WRAP: stencilOp = GL_INCR_WRAP;	break;
	case StencilOp::DECREMENT_WRAP: stencilOp = GL_DECR_WRAP;	break;
	default:
		break;
	}
	return stencilOp;
}


void OpenGLRenderer::OperateAfterStencilTest(int face, StencilOp stencilFailed, StencilOp depthFailed, StencilOp depthPassed) {
	int sFailed = GetStencilOp(stencilFailed);
	int dFailed = GetStencilOp(depthFailed);
	int dPassed = GetStencilOp(depthPassed);

	pDevice->glStencilOpSeparate(face, sFailed, dFailed, dPassed);
}

void OpenGLRenderer::EnableStencilTest(bool bEnable) {
	bEnable ? glEnable(GL_STENCIL_TEST) : glDisable(GL_STENCIL_TEST);
}
void OpenGLRenderer::SetStencilFunc(int face, eCompare _stencilFunc, int ref, unsigned int mask) {
	int stencilFunc = GetDepthCompare(_stencilFunc);
	pDevice->glStencilFuncSeparate(face, stencilFunc, ref, mask);
}

void OpenGLRenderer::SetDepthFunc(eCompare _depthCmp) {
	int depthFunc = GetDepthCompare(_depthCmp);
	glDepthFunc(depthFunc);
}


void OpenGLRenderer::EnableCulling(bool bEnable) {
	bEnable == true ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
}

bool OpenGLRenderer::BeginRender() {
	pDevice->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);
	return true;
}

void OpenGLRenderer::EndRender() {
	pDevice->EndScene();
}
