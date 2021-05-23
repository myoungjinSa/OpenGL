#include "Renderer.h"
#include "Camera.h"
#include "Scene.h"
#include "Shader.h"
#include "Object.h"
#include "Texture.h"
#include "String/String.h"
#include "Mesh.h"
#include <fstream>
#include <stdarg.h>

Renderer::Renderer() 
	:pGL(nullptr),
	drawMode(DrawMode::TRIANGLES)
{
}

Renderer::~Renderer() {

}

bool Renderer::Initialize(OpenGL* pOpenGL, HWND _hWnd) {
	//Store a pointer to ther OpenGL class object.
	pGL = pOpenGL;
	if (!pGL)
		return false;
	
	hWnd = _hWnd;

	return true;
}


void Renderer::Shutdown(unsigned int shaderProgram, unsigned int vertexShader, unsigned int fragmentShader) {
	pGL->glDetachShader(shaderProgram, vertexShader);
	pGL->glDetachShader(shaderProgram, fragmentShader);

	pGL->glDeleteShader(vertexShader);
	pGL->glDeleteShader(fragmentShader);
	
	pGL->glDeleteProgram(shaderProgram);

	TextureLoader::Release();
}

bool Renderer::AllocateVertexBuffer(unsigned int vertexArrayId, unsigned int vertexBufferId, void* vertexData, VertexBufferBindCallback* pBindFunction, unsigned int numVertices, unsigned int sizeofVertex) {
	pGL->glGenVertexArrays(1, &vertexArrayId);
	pGL->glBindVertexArray(vertexArrayId);
	pGL->glGenBuffers(1, &vertexBufferId);

	pBindFunction(*pGL, vertexData, vertexBufferId, numVertices, sizeofVertex);
	return true;
}


bool Renderer::AllocateIndexBuffer(unsigned int indexBufferId, size_t indexCount, unsigned int* indexData) {
	pGL->glGenBuffers(1, &indexBufferId);

	pGL->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
	pGL->glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned int), indexData, GL_STATIC_DRAW);
	return true;
}


bool Renderer::AllocateTextures(unsigned int textureUnit, unsigned int textureId, unsigned int textureCount) {
	//Set the unique texture unit in which to store the data
	pGL->glActiveTexture(GL_TEXTURE0 + textureUnit);

	//Generate an ID for the texture.
	glGenTextures(textureCount, &textureId);
	return true;
}

void Renderer::SetSampleMode(bool wrapMode) {
	if (wrapMode) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	}
}

void Renderer::SetFiltering() {
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	pGL->glGenerateMipmap(GL_TEXTURE_2D);
}
void Renderer::BindTexture(unsigned int width, unsigned int height, unsigned int textureUnit, unsigned int textureId, unsigned char* pPixelData) {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pPixelData);
}

void Renderer::DisableVertexAttribArray(size_t vertexAttribCount) {
	for (size_t iVertexAttrib = 0; iVertexAttrib < vertexAttribCount; iVertexAttrib++) {
		pGL->glDisableVertexAttribArray(iVertexAttrib);
	}
}


void Renderer::ReleaseVertexArray(unsigned int vertexArrayId, size_t arrayCount) {
	pGL->glBindVertexArray(0);
	pGL->glDeleteVertexArrays(arrayCount, &vertexArrayId);
}
void Renderer::ReleaseVertexBuffers(unsigned int vertexBufferId, size_t bufferCount) {
	pGL->glBindBuffer(GL_ARRAY_BUFFER, 0);
	pGL->glDeleteBuffers(bufferCount, &vertexBufferId);
}
void Renderer::ReleaseIndexBuffers(unsigned int indexBufferId, size_t indexCount) {
	pGL->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	pGL->glDeleteBuffers(indexCount, &indexBufferId);
}

void Renderer::DrawVertexBuffer(unsigned int vertexArrayId, size_t startPos, size_t vertexCount) {
	pGL->glBindVertexArray(vertexArrayId);

	glDrawArrays(GL_TRIANGLES, startPos, vertexCount);
}

void Renderer::DrawIndexBuffer(unsigned int vertexArrayId, size_t indexCount) {
	pGL->glBindVertexArray(vertexArrayId);

	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
}


bool Renderer::BeginRender() {
	pGL->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	return true;
}

void Renderer::EndRender() {
	pGL->EndScene();
}

unsigned int Renderer::CreateShader() {
	return pGL->glCreateProgram();
}
void Renderer::SetShader(unsigned int shaderPrgram) {
	pGL->glUseProgram(shaderPrgram);
}
bool Renderer::CompileVertexShader(const char* vsFilename, unsigned int& vertexShader) {
	const char* vertexShaderBuffer;
	vertexShaderBuffer = LoadShaderSourceFile(vsFilename);
	if (!vertexShaderBuffer)
	{
		return false;
	}
	vertexShader = pGL->glCreateShader(GL_VERTEX_SHADER);
	pGL->glShaderSource(vertexShader, 1, &vertexShaderBuffer, NULL);
	delete vertexShaderBuffer;
	vertexShaderBuffer = nullptr;

	int status = 0;
	pGL->glCompileShader(vertexShader);
	pGL->glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
	if (status != 1) {
		// If it did not compile then write the syntax error message out to a text file for review.
		OutputShaderErrorMessage(*pGL, vertexShader, const_cast<char*>(vsFilename));
		return false;
	}
}

bool Renderer::CompileFragmentShader(const char* fsFilename, unsigned int& fragmentShader) {
	const char* fragmentShaderBuffer;
	fragmentShaderBuffer = LoadShaderSourceFile(fsFilename);
	if (!fragmentShaderBuffer)
	{
		return false;
	}
	fragmentShader = pGL->glCreateShader(GL_VERTEX_SHADER);
	pGL->glShaderSource(fragmentShader, 1, &fragmentShaderBuffer, NULL);
	delete fragmentShaderBuffer;
	fragmentShaderBuffer = nullptr;

	int status = 0;
	pGL->glCompileShader(fragmentShader);
	pGL->glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
	if (status != 1) {

	}
}

bool Renderer::SetShaderParameter(unsigned int shaderProgram, std::array<std::array<float, 4>, 4>& matrix, String&& variableName) {
	_ASSERT(pGL);
	unsigned int location = pGL->glGetUniformLocation(shaderProgram, variableName.c_str());
	if (location == -1)
		return false;

	pGL->glUniformMatrix4fv(location, 1, false, matrix[0].data());
}

bool Renderer::SetShaderParameter(unsigned int shaderProgram, std::array<std::array<float, 3>, 3>& matrix, String&& variableName) {
	_ASSERT(pGL);
	unsigned int location = pGL->glGetUniformLocation(shaderProgram, variableName.c_str());
	if (location == -1)
		return false;

	pGL->glUniform3fv(location, 1, matrix[0].data());
}

bool Renderer::SetShaderParameter(unsigned int shaderProgram, int integer, String&& variableName) {
	_ASSERT(pGL);
	unsigned int location = pGL->glGetUniformLocation(shaderProgram, variableName.c_str());
	if (location == -1)
		return false;

	pGL->glUniform1i(location, integer);
}
bool Renderer::SetShaderParameter(unsigned int shaderProgram, std::array<std::array<float, 4>, 4>& matrix,const String& variableName) {
	_ASSERT(pGL);
	unsigned int location = pGL->glGetUniformLocation(shaderProgram, variableName.c_str());
	if (location == -1)
		return false;

	pGL->glUniformMatrix4fv(location, 1, false, matrix[0].data());
}

bool Renderer::SetShaderParameter(unsigned int shaderProgram, std::array<std::array<float, 3>, 3>& matrix, const String& variableName) {
	_ASSERT(pGL);
	unsigned int location = pGL->glGetUniformLocation(shaderProgram, variableName.c_str());
	if (location == -1)
		return false;

	pGL->glUniform3fv(location, 1, matrix[0].data());
}

bool Renderer::SetShaderParameter(unsigned int shaderProgram, int integer, const String& variableName) {
	_ASSERT(pGL);
	unsigned int location = pGL->glGetUniformLocation(shaderProgram, variableName.c_str());
	if (location == -1)
		return false;

	pGL->glUniform1i(location, integer);
}
bool Renderer::BindVertexAttrib(unsigned int shaderProgram, unsigned int vertexShader, unsigned int fragmentShader, int vertexArgs, ...) {
	
	pGL->glAttachShader(shaderProgram, vertexShader);
	pGL->glAttachShader(shaderProgram, fragmentShader);

	va_list ap;
	va_start(ap, vertexArgs);
	String attrib;
	for (size_t iAttrib = 0; iAttrib < vertexArgs; iAttrib++) {
		attrib = va_arg(ap, String);

		pGL->glBindAttribLocation(shaderProgram, iAttrib, attrib.c_str());
	}
	pGL->glLinkProgram(shaderProgram);

	GLint status;
	pGL->glGetProgramiv(shaderProgram, GL_LINK_STATUS, &status);
	if (status != GL_TRUE) {
		OutputLinkErrorMessage(*pGL, shaderProgram);
		return false;
	}
	return true;
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

void Renderer::OutputLinkErrorMessage(OpenGL& gl, unsigned int programId) {
	int logSize = 0;
	gl.glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &logSize);

	//Increment the size by one to handle also the null terminator.
	logSize++;
	char* infoLog = new char[logSize];
	if (!infoLog)
		return;

	gl.glGetProgramInfoLog(programId, logSize, NULL, infoLog);
	std::ofstream fout;
	fout.open("linker-error.txt");
	for (size_t iLog = 0; iLog < logSize; iLog++)
		fout << infoLog[iLog];

	fout.close();

	MessageBox(hWnd, L"Error compiling linker. Check linker-error.txt for message", L"Linker Error", MB_OK);
}

void Renderer::OutputShaderErrorMessage(OpenGL& gl, unsigned int shaderId, char* shaderFilename) {
	std::ofstream fout;

	char* infoLog = nullptr;
	int logSize = 0;
	gl.glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &logSize);

	//Increment the size by one to handle also the null terminator.
	logSize++;

	//Create a char buffer to hold the info log.
	infoLog = new char[logSize];
	if (!infoLog)
		return;

	gl.glGetShaderInfoLog(shaderId, logSize, NULL, infoLog);

	//Open a file to write the error message to.
	fout.open("shader-error.txt");

	for (size_t iLog = 0; iLog < logSize; iLog++) {
		fout << infoLog[iLog];
	}

	fout.close();

	//Convert the shader filename to a widechar string
	size_t convertedChars = 0;
	wchar_t newString[128];
	size_t error = mbstowcs_s(&convertedChars, newString, 128, shaderFilename, 128);
	if (error != 0)
		return;

	MessageBox(hWnd, L"Error compile shader. Check shader-error.txt for message.", newString, MB_OK);
}