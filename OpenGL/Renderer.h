#pragma once
#include "Common.h"
#include "OpenGL.h"
/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class OpenGL;
class Object;
class PhongShader;
class ColorShader;
class Camera;
class Scene;
class String;
class Renderer
{
public:
	enum class DrawMode : unsigned int {
		QUADS,
		QUAD_STRIP,
		POINTS,
		LINES,
		LINE_LOOP,
		POLYGON,
		TRIANGLES,
		NUM_DRAW_MODES
	};

	Renderer();
	Renderer(const Renderer&) = delete;
	~Renderer();

	bool Initialize(OpenGL* pOpenGL, HWND hWnd); 
	void Shutdown(unsigned int shaderProgram, unsigned int vertexShader, unsigned int fragmentShader);
	bool Frame();

	void Attach(Scene& scene);
	void Detach();

	unsigned int CreateShader();
	void SetShader(unsigned int shaderProgram);
	bool CompileVertexShader(const char* vsFilename, unsigned int& vertexShader);
	bool CompileFragmentShader(const char* fsFilename, unsigned int& fragmentShader);
	bool BindVertexAttrib(unsigned int shaderProgram, unsigned int vertexShader, unsigned int fragmentShader, int vertexArgs, ...);
	
	bool SetShaderParameter(unsigned int shaderProgram, std::array<std::array<float, 4>, 4>& matrix, const String& variableName);
	bool SetShaderParameter(unsigned int shaderProgram, std::array<std::array<float, 3>, 3>& matrix, const String& variableName);
	bool SetShaderParameter(unsigned int shaderProgram, int integer, const String& variableName);

	bool SetShaderParameter(unsigned int shaderProgram, std::array<std::array<float, 4>, 4>& matrix, String&& variableName);
	bool SetShaderParameter(unsigned int shaderProgram, std::array<std::array<float, 3>, 3>& matrix, String&& variableName);
	bool SetShaderParameter(unsigned int shaderProgram, int integer, String&& variableName);

private:
	bool Render(Scene& scene);

	void OutputLinkErrorMessage(OpenGL& gl, unsigned int programId);
	void OutputShaderErrorMessage(OpenGL& gl, unsigned int shaderId, char* shaderFilename);
	char* LoadShaderSourceFile(const char* filename);
private:
	OpenGL* pGL;
	//PhongShader* pShader;
	Scene* pTargetScene;
	HWND hWnd;
};

