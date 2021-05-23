#include "Shader.h"
#include "Object.h"
#include "Renderer.h"
#include "String/String.h"
#include <fstream>


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
//char* Shader::LoadShaderSourceFile(const char* filename) {
//	std::ifstream fin;
//
//	//Open the shader source file.
//	fin.open(filename);
//	if (fin.fail())
//		return 0;
//
//	int fileSize = 0;
//
//	//Read the first element of the file
//	char input;
//	fin.get(input);
//
//	//Count the number of elements in the text file
//	while (!fin.eof()) {
//		fileSize++;
//		fin.get(input);
//	}
//	//Close the file for now
//	fin.close();
//
//	//Initialize the buffer to read the shader source file into.
//	char* buffer = new char[fileSize + 1];
//	if (!buffer) {
//		return 0;
//	}
//
//	fin.open(filename);
//	fin.read(buffer, fileSize);
//	fin.close();
//
//	buffer[fileSize] = '\0';
//
//	return buffer;
//}

//void Shader::OutputLinkErrorMessage(OpenGL& gl, HWND hWnd, unsigned int programId) {
//	int logSize = 0;
//	gl.glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &logSize);
//
//	//Increment the size by one to handle also the null terminator.
//	logSize++;
//	char* infoLog = new char[logSize];
//	if (!infoLog)
//		return;
//
//	gl.glGetProgramInfoLog(programId, logSize, NULL, infoLog);
//	std::ofstream fout;
//	fout.open("linker-error.txt");
//	for (size_t iLog = 0; iLog < logSize; iLog++)
//		fout << infoLog[iLog];
//
//	fout.close();
//
//	MessageBox(hWnd, L"Error compiling linker. Check linker-error.txt for message", L"Linker Error", MB_OK);
//}


//void Shader::OutputShaderErrorMessage(OpenGL& gl, HWND hWnd, unsigned int shaderId, char* shaderFilename) {
//	std::ofstream fout;
//
//	char* infoLog = nullptr;
//	int logSize = 0;
//	gl.glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &logSize);
//
//	//Increment the size by one to handle also the null terminator.
//	logSize++;
//
//	//Create a char buffer to hold the info log.
//	infoLog = new char[logSize];
//	if (!infoLog)
//		return;
//
//	gl.glGetShaderInfoLog(shaderId, logSize, NULL, infoLog);
//
//	//Open a file to write the error message to.
//	fout.open("shader-error.txt");
//
//	for (size_t iLog = 0; iLog < logSize; iLog++) {
//		fout << infoLog[iLog];
//	}
//
//	fout.close();
//
//	//Convert the shader filename to a widechar string
//	size_t convertedChars = 0;
//	wchar_t newString[128];
//	size_t error = mbstowcs_s(&convertedChars, newString, 128, shaderFilename, 128);
//	if (error != 0)
//		return;
//
//	MessageBox(hWnd, L"Error compile shader. Check shader-error.txt for message.", newString, MB_OK);
//}

void Shader::SetShader(Renderer& renderer) {
	renderer.SetShader(shaderProgram);
}

void Shader::ShutdownShader(Renderer& renderer) {
	renderer.Shutdown(shaderProgram, vertexShader, fragmentShader);
}

//bool Shader::CompileShader(const char* vsFilename, const char* fsFilename, OpenGL& gl, HWND hWnd) {
//	const char* vertexShaderBuffer;
//	const char* fragmentShaderBuffer;
//	int status;
//
//
//	// Load the vertex shader source file into a text buffer.
//	vertexShaderBuffer = LoadShaderSourceFile(vsFilename);
//	if (!vertexShaderBuffer)
//	{
//		return false;
//	}
//
//	// Load the fragment shader source file into a text buffer.
//	fragmentShaderBuffer = LoadShaderSourceFile(fsFilename);
//	if (!fragmentShaderBuffer)
//	{
//		return false;
//	}
//
//	// Create a vertex and fragment shader object.
//	vertexShader = gl.glCreateShader(GL_VERTEX_SHADER);
//	fragmentShader = gl.glCreateShader(GL_FRAGMENT_SHADER);
//
//	// Copy the shader source code strings into the vertex and fragment shader objects.
//	gl.glShaderSource(vertexShader, 1, &vertexShaderBuffer, NULL);
//	gl.glShaderSource(fragmentShader, 1, &fragmentShaderBuffer, NULL);
//
//	// Release the vertex and fragment shader buffers.
//	delete[] vertexShaderBuffer;
//	vertexShaderBuffer = 0;
//
//	delete[] fragmentShaderBuffer;
//	fragmentShaderBuffer = 0;
//
//	// Compile the shaders.
//	gl.glCompileShader(vertexShader);
//	gl.glCompileShader(fragmentShader);
//
//	// Check to see if the vertex shader compiled successfully.
//	gl.glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
//	if (status != 1)
//	{
//		// If it did not compile then write the syntax error message out to a text file for review.
//		OutputShaderErrorMessage(gl, hWnd, vertexShader, const_cast<char*>(vsFilename));
//		return false;
//	}
//
//	// Check to see if the fragment shader compiled successfully.
//	gl.glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
//	if (status != 1)
//	{
//		// If it did not compile then write the syntax error message out to a text file for review.
//		OutputShaderErrorMessage(gl, hWnd, fragmentShader, const_cast<char*>(fsFilename));
//		return false;
//	}
//	return true;
//}


ColorShader::ColorShader() 
	:Shader()
{

}
ColorShader::~ColorShader() {

}

bool ColorShader::Initialize(Renderer& renderer, HWND hWnd) {
	bool result;
	result = InitializeShader("color.vs", "color.ps", renderer, hWnd);

	Object* pObject = new Object();
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

void ColorShader::SetShader(Renderer& renderer) {
	Shader::SetShader(renderer);
}

void ColorShader::Render(Renderer& renderer) {
	for (const auto& obj : objects)
		obj->Render(renderer);
}

bool ColorShader::InitializeShader(const char* vsFilename, const char* fsFilename, Renderer& renderer, HWND hWnd) {
	
	//if (!CompileShader(vsFilename, fsFilename, gl, hWnd))
	//	return false;

	////Create a shader program object.
	//shaderProgram = gl.glCreateProgram();

	////Attach the vertex and fragment shader to the program object.
	//gl.glAttachShader(shaderProgram, vertexShader);
	//gl.glAttachShader(shaderProgram, fragmentShader);

	////Bind the shader input variables.
	//gl.glBindAttribLocation(shaderProgram, 0, "inputPosition");
	//gl.glBindAttribLocation(shaderProgram, 1, "inputColor");

	////Link the shader Program
	//gl.glLinkProgram(shaderProgram);

	//GLint status;
	//gl.glGetProgramiv(shaderProgram, GL_LINK_STATUS, &status);
	//if (status != 1)
	//{
	//	OutputLinkErrorMessage(gl, hWnd, shaderProgram);
	//	return false;
	//}
	//return true;
	shaderProgram = renderer.CreateShader();

	if (!renderer.CompileVertexShader(vsFilename, vertexShader))
		return false;

	if (!renderer.CompileFragmentShader(fsFilename, fragmentShader))
		return false;

	String inputPosition("inputPosition");
	String inputColor("inputColor");
	return renderer.BindVertexAttrib(shaderProgram, vertexShader, fragmentShader, 2, inputPosition, inputColor);
}

bool ColorShader::SetShaderParameters(Renderer& renderer, std::array<std::array<float, 4>, 4>& worldMatrix, std::array<std::array<float, 4>, 4>& viewMatrix, std::array<std::array<float, 4>, 4>& projectionMatrix) {
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

//bool ColorShader::SetShaderParameters(Renderer& renderer, float* worldMatrix, float* viewMatrix, float* projectionMatrix) {
//	unsigned int location = gl.glGetUniformLocation(shaderProgram, "worldMatrix");
//	if (location == -1)
//		return false;
//
//	gl.glUniformMatrix4fv(location, 1, false, worldMatrix);
//
//	//Set the view matrix in the vertex shader
//
//	location = gl.glGetUniformLocation(shaderProgram, "viewMatrix");
//	if (location == -1)
//		return false;
//	
//	gl.glUniformMatrix4fv(location, 1, false, viewMatrix);
//
//	//Set the projection matrix in the vertex shader
//	location = gl.glGetUniformLocation(shaderProgram, "projectionMatrix");
//	if (location == -1)
//		return false;
//
//	gl.glUniformMatrix4fv(location, 1, false, projectionMatrix);
//
//	return true;
//}



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

void PhongShader::Render(Renderer& renderer) {
	for (const auto& obj : objects)
		obj->Render(renderer);
}
void PhongShader::SetShader(Renderer& renderer) {
	renderer.SetShader(shaderProgram);
}

void PhongShader::Shutdown(Renderer& renderer) {
	Shader::Shutdown(renderer);
	ShutdownShader(renderer);
}

bool PhongShader::InitializeShader(const char* vsFilename, const char* fsFilename, Renderer& renderer, HWND hWnd) {
	//shaderProgram = gl.glCreateProgram();
	shaderProgram = renderer.CreateShader();

	if (!renderer.CompileVertexShader(vsFilename, vertexShader))
		return false;

	if (!renderer.CompileFragmentShader(fsFilename, fragmentShader))
		return false;

	/*if (!CompileShader(vsFilename, fsFilename, gl, hWnd))
		return false;*/


	/*gl.glAttachShader(shaderProgram, vertexShader);
	gl.glAttachShader(shaderProgram, fragmentShader);
	
	gl.glBindAttribLocation(shaderProgram, 0, "inputPosition");
	gl.glBindAttribLocation(shaderProgram, 1, "inputTexCoord");
	gl.glBindAttribLocation(shaderProgram, 2, "inputNormal");

	gl.glLinkProgram(shaderProgram);

	GLint status;
	gl.glGetProgramiv(shaderProgram, GL_LINK_STATUS, &status);
	if (status != GL_TRUE) {
		OutputLinkErrorMessage(gl, hWnd, shaderProgram);
		return false;
	}
	return true;*/
	String inputPosition("inputPosition");
	String inputTexCoord("inputTexCoord");
	String inputNormal("inputNormal");
	return renderer.BindVertexAttrib(shaderProgram, vertexShader, fragmentShader, 3, inputPosition, inputTexCoord, inputNormal);
}

bool PhongShader::SetShaderParameters(Renderer& renderer, std::array<std::array<float, 4>, 4>& worldMatrix, std::array<std::array<float, 4>, 4>&viewMatrix,
	std::array<std::array<float, 4>, 4>& projectionMatrix, std::array<std::array<float, 3>, 3>& lightDirection, std::array<std::array<float, 3>, 3>& diffuseAlbedo, std::array<std::array<float, 4>, 4>& ambientAlbedo, std::array<std::array<float, 3>, 3>& specular, int textureUnit) {

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

	if (!renderer.SetShaderParameter(shaderProgram, diffuseAlbedo, std::move(String("diffuseAlbedo")))) {
		return false;
	}
	if (!renderer.SetShaderParameter(shaderProgram, ambientAlbedo, std::move(String("ambientAlbedo")))) {
		return false;
	}
	if (!renderer.SetShaderParameter(shaderProgram, specular, std::move(String("specularAlbedo")))) {
		return false;
	}
	if (!renderer.SetShaderParameter(shaderProgram, textureUnit, std::move(String("shaderTexture")))) {
		return false;
	}
	return true;
}

//bool PhongShader::SetShaderParameters(Renderer& renderer, float* worldMatrix, float* viewMatrix, float* projectionMatrix, int textureUnit, 
//	float* lightDirection, float* diffuseAlbedo, float* ambientAlbedo, float* specularAlbedo) {
//	int location = 0;
//	
//	location = gl.glGetUniformLocation(shaderProgram, "worldMatrix");
//	if (location == -1)
//		return false;
//
//	gl.glUniformMatrix4fv(location, 1, false, worldMatrix);
//
//	//Set the view matrix in the vertex shader.
//	location = gl.glGetUniformLocation(shaderProgram, "viewMatrix");
//	if (location == -1)
//		return false;
//
//	gl.glUniformMatrix4fv(location, 1, false, viewMatrix);
//
//	//Set the projection matrix in the vertex shader.
//	location = gl.glGetUniformLocation(shaderProgram, "projectionMatrix");
//	if (location == -1)
//		return false;
//
//	gl.glUniformMatrix4fv(location, 1, false, projectionMatrix);
//	
//	location = gl.glGetUniformLocation(shaderProgram, "lightDirection");
//	if (location == -1)
//		return false;
//
//	gl.glUniform3fv(location, 1, lightDirection);
//
//	//Set the texture in the pixel shader to use the data from the first texture unit.
//	location = gl.glGetUniformLocation(shaderProgram, "shaderTexture");
//	if (location == -1)
//		return false;
//	gl.glUniform1i(location, textureUnit);
//
//	location = gl.glGetUniformLocation(shaderProgram, "diffuseAlbedo");
//	if (location == -1)
//		return false;
//
//	gl.glUniform3fv(location, 1, diffuseAlbedo);
//	
//	location = gl.glGetUniformLocation(shaderProgram, "ambientAlbedo");
//	if (location == -1)
//		return false;
//	gl.glUniform4fv(location, 1, ambientAlbedo);
//
//
//	location = gl.glGetUniformLocation(shaderProgram, "specularAlbedo");
//	if (location == -1) {
//		return false;
//	}
//	gl.glUniform3fv(location, 1, specularAlbedo);
//
//
//	return true;
//}
//
