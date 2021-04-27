#include "Shader.h"
#include "Object.h"
#include <fstream>

Shader::Shader() 
	:vertexShader(0), fragmentShader(0), shaderProgram(0)
{
	objects.clear();
}

Shader::~Shader() {

}

char* Shader::LoadShaderSourceFile(const char* filename) {
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

void Shader::OutputLinkErrorMessage(OpenGL* pGL, HWND hWnd, unsigned int programId) {
	int logSize = 0;
	pGL->glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &logSize);

	//Increment the size by one to handle also the null terminator.
	logSize++;
	char* infoLog = new char[logSize];
	if (!infoLog)
		return;

	pGL->glGetProgramInfoLog(programId, logSize, NULL, infoLog);
	std::ofstream fout;
	fout.open("linker-error.txt");
	for (size_t iLog = 0; iLog < logSize; iLog++)
		fout << infoLog[iLog];

	fout.close();

	MessageBox(hWnd, L"Error compiling linker. Check linker-error.txt for message", L"Linker Error", MB_OK);
}


void Shader::OutputShaderErrorMessage(OpenGL* pGL, HWND hWnd, unsigned int shaderId, char* shaderFilename) {
	std::ofstream fout;

	char* infoLog = nullptr;
	int logSize = 0;
	pGL->glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &logSize);

	//Increment the size by one to handle also the null terminator.
	logSize++;

	//Create a char buffer to hold the info log.
	infoLog = new char[logSize];
	if (!infoLog)
		return;

	pGL->glGetShaderInfoLog(shaderId, logSize, NULL, infoLog);

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


void Shader::ShutdownShader(OpenGL* pGL) {
	pGL->glDetachShader(shaderProgram, vertexShader);
	pGL->glDetachShader(shaderProgram, fragmentShader);

	pGL->glDeleteShader(vertexShader);
	pGL->glDeleteShader(fragmentShader);

	pGL->glDeleteProgram(shaderProgram);
}

bool Shader::CompileShader(const char* vsFilename, const char* fsFilename, OpenGL* pGL, HWND hWnd) {
	const char* vertexShaderBuffer;
	const char* fragmentShaderBuffer;
	int status;


	// Load the vertex shader source file into a text buffer.
	vertexShaderBuffer = LoadShaderSourceFile(vsFilename);
	if (!vertexShaderBuffer)
	{
		return false;
	}

	// Load the fragment shader source file into a text buffer.
	fragmentShaderBuffer = LoadShaderSourceFile(fsFilename);
	if (!fragmentShaderBuffer)
	{
		return false;
	}

	// Create a vertex and fragment shader object.
	vertexShader = pGL->glCreateShader(GL_VERTEX_SHADER);
	fragmentShader = pGL->glCreateShader(GL_FRAGMENT_SHADER);

	// Copy the shader source code strings into the vertex and fragment shader objects.
	pGL->glShaderSource(vertexShader, 1, &vertexShaderBuffer, NULL);
	pGL->glShaderSource(fragmentShader, 1, &fragmentShaderBuffer, NULL);

	// Release the vertex and fragment shader buffers.
	delete[] vertexShaderBuffer;
	vertexShaderBuffer = 0;

	delete[] fragmentShaderBuffer;
	fragmentShaderBuffer = 0;

	// Compile the shaders.
	pGL->glCompileShader(vertexShader);
	pGL->glCompileShader(fragmentShader);

	// Check to see if the vertex shader compiled successfully.
	pGL->glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
	if (status != 1)
	{
		// If it did not compile then write the syntax error message out to a text file for review.
		OutputShaderErrorMessage(pGL, hWnd, vertexShader, const_cast<char*>(vsFilename));
		return false;
	}

	// Check to see if the fragment shader compiled successfully.
	pGL->glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
	if (status != 1)
	{
		// If it did not compile then write the syntax error message out to a text file for review.
		OutputShaderErrorMessage(pGL, hWnd, fragmentShader, const_cast<char*>(fsFilename));
		return false;
	}
	return true;
}


ColorShader::ColorShader() 
	:Shader()
{

}
ColorShader::~ColorShader() {

}

bool ColorShader::Initialize(OpenGL* pGL, HWND hWnd) {
	bool result;
	result = InitializeShader("color.vs", "color.ps", pGL, hWnd);

	Object* pObject = new Object();
	objects.emplace_back(pObject);

	for (const auto& obj : objects) {
		if (!obj->Initialize(pGL)) {
			MessageBox(hWnd, L"Could not initialize the model object", L"Error", MB_OK);
			return false;
		}
	}

	if (!result)
		return false;
	return true;
}

void ColorShader::Shutdown(OpenGL* pGL) {
	ShutdownShader(pGL);
}

void ColorShader::SetShader(OpenGL* pGL) {
	if (!pGL) {
		return;
	}
	pGL->glUseProgram(shaderProgram);
}

void ColorShader::Render(OpenGL* pGL) {
	for (const auto& obj : objects)
		obj->Render(pGL);
}

bool ColorShader::InitializeShader(const char* vsFilename, const char* fsFilename, OpenGL* pGL, HWND hWnd) {
	
	if (!CompileShader(vsFilename, fsFilename, pGL, hWnd))
		return false;

	//Create a shader program object.
	shaderProgram = pGL->glCreateProgram();

	//Attach the vertex and fragment shader to the program object.
	pGL->glAttachShader(shaderProgram, vertexShader);
	pGL->glAttachShader(shaderProgram, fragmentShader);

	//Bind the shader input variables.
	pGL->glBindAttribLocation(shaderProgram, 0, "inputPosition");
	pGL->glBindAttribLocation(shaderProgram, 1, "inputColor");

	//Link the shader Program
	pGL->glLinkProgram(shaderProgram);

	GLint status;
	pGL->glGetProgramiv(shaderProgram, GL_LINK_STATUS, &status);
	if (status != 1)
	{
		OutputLinkErrorMessage(pGL, hWnd, shaderProgram);
		return false;
	}
	return true;
}


bool ColorShader::SetShaderParameters(OpenGL* pGL, float* worldMatrix, float* viewMatrix, float* projectionMatrix) {
	unsigned int location = pGL->glGetUniformLocation(shaderProgram, "worldMatrix");
	if (location == -1)
		return false;

	pGL->glUniformMatrix4fv(location, 1, false, worldMatrix);

	//Set the view matrix in the vertex shader

	location = pGL->glGetUniformLocation(shaderProgram, "viewMatrix");
	if (location == -1)
		return false;
	
	pGL->glUniformMatrix4fv(location, 1, false, viewMatrix);

	//Set the projection matrix in the vertex shader
	location = pGL->glGetUniformLocation(shaderProgram, "projectionMatrix");
	if (location == -1)
		return false;

	pGL->glUniformMatrix4fv(location, 1, false, projectionMatrix);

	return true;
}



/////////////////////////////Texture Shader//////////////////////////////////////////////////////////////////
TextureShader::TextureShader() 
	: Shader()
{

}

TextureShader::~TextureShader() {

}

bool TextureShader::Initialize(OpenGL* pGL, HWND hWnd) {
	bool result;
	result = InitializeShader("texture.vs", "texture.ps", pGL, hWnd);

	Object* pObject = new Object();
	objects.emplace_back(pObject);

	for (const auto& obj : objects) {
		if (!obj->Initialize(pGL)) {
			MessageBox(hWnd, L"Could not initialize the model object", L"Error", MB_OK);
			return false;
		}
	}


	if (!result)
		return false;
	return true;
}

void TextureShader::SetShader(OpenGL* pGL) {
	if (!pGL) {
		return;
	}
	pGL->glUseProgram(shaderProgram);
}

void TextureShader::Render(OpenGL* pGL) {
	for (const auto& obj : objects)
		obj->Render(pGL);	
}

void TextureShader::Shutdown(OpenGL* pGL) {
	ShutdownShader(pGL);
}

bool TextureShader::InitializeShader(const char* vsFilename, const char* fsFilename, OpenGL* pGL, HWND hWnd) {
	if (!CompileShader(vsFilename, fsFilename, pGL, hWnd))
		return false;

	shaderProgram = pGL->glCreateProgram();

	pGL->glAttachShader(shaderProgram, vertexShader);
	pGL->glAttachShader(shaderProgram, fragmentShader);

	pGL->glBindAttribLocation(shaderProgram, 0, "inputPosition");
	pGL->glBindAttribLocation(shaderProgram, 1, "inputTexCoord");

	pGL->glLinkProgram(shaderProgram);

	GLint status;
	pGL->glGetProgramiv(shaderProgram, GL_LINK_STATUS, &status);
	if (status != GL_TRUE) {
		OutputLinkErrorMessage(pGL, hWnd, shaderProgram);
		return false;
	}
	return true;
}

bool TextureShader::SetShaderParameters(OpenGL* pGL, float* worldMatrix, float* viewMatrix, float* projectionMatrix, int textureUnit) {
	unsigned int location;
	
	location = pGL->glGetUniformLocation(shaderProgram, "worldMatrix");
	if (location == -1)
		return false;

	pGL->glUniformMatrix4fv(location, 1, false, worldMatrix);

	//Set the view matrix in the vertex shader.
	location = pGL->glGetUniformLocation(shaderProgram, "viewMatrix");
	if (location == -1)
		return false;

	pGL->glUniformMatrix4fv(location, 1, false, viewMatrix);

	//Set the projection matrix in the vertex shader.
	location = pGL->glGetUniformLocation(shaderProgram, "projectionMatrix");
	if (location == -1)
		return false;

	pGL->glUniformMatrix4fv(location, 1, false, projectionMatrix);

	//Set the texture in the pixel shader to use the data from the first texture unit.
	location = pGL->glGetUniformLocation(shaderProgram, "shaderTexture");
	if (location == -1)
		return false;
	pGL->glUniform1i(location, textureUnit);

	return true;
}