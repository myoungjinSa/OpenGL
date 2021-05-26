#pragma once
#include "Common.h"
#include "OpenGL.h"
#include "Mesh.h"
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

	unsigned int CreateShader();
	void SetShader(unsigned int shaderProgram);
	bool CompileVertexShader(const char* vsFilename, unsigned int& vertexShader);
	bool CompileFragmentShader(const char* fsFilename, unsigned int& fragmentShader);
	bool BindVertexAttrib(unsigned int shaderProgram, unsigned int vertexShader, unsigned int fragmentShader, int vertexArgs, ...);
	
	void SetDrawMode(Renderer::DrawMode drawMode);
	Renderer::DrawMode GetDrawMode() const { return drawMode; }

	bool SetShaderParameter(unsigned int shaderProgram, Matrix<float, 4, 4>& matrix, const String& variableName);
	bool SetShaderParameter(unsigned int shaderProgram, Vec4f& vec4, const String& variableName);
	bool SetShaderParameter(unsigned int shaderProgram, Vec3f& vec3, const String& variableName);
	bool SetShaderParameter(unsigned int shaderProgram, int integer, const String& variableName);


	bool SetShaderParameter(unsigned int shaderProgram, Matrix<float, 4, 4>& matrix, String&& variableName);
	bool SetShaderParameter(unsigned int shaderProgram, Vec4f& vec4, String&& variableName);
	bool SetShaderParameter(unsigned int shaderProgram, Vec3f& vec3, String&& variableName);
	bool SetShaderParameter(unsigned int shaderProgram, int integer, String&& variableName);


	bool AllocateVertexBuffer(unsigned int& vertexArrayId, unsigned int& vertexBufferId, void* vertexData, VertexBufferBindCallback* pBindFunction, unsigned int numVertices, unsigned int sizeofVertex);
	bool AllocateIndexBuffer(unsigned int& indexBufferId, size_t indexCount, unsigned int* indexData);
	
	bool AllocateTextures(unsigned int textureUnit, unsigned int& textureID, unsigned int textureCount);
	void BindTexture(unsigned int width, unsigned int height, unsigned int textureId, unsigned char* pPixelData);
	void SetSampleMode(bool wrapMode);
	void SetFiltering();


	void DisableVertexAttribArray(size_t vertexAttribCount);
	void ReleaseVertexBuffers(unsigned int& vertexBufferId, size_t bufferCount);
	void ReleaseIndexBuffers(unsigned int& vertexBufferId, size_t bufferCount);
	void ReleaseVertexArray(unsigned int& vertedxArrayId, size_t arrayCount);
	void DrawVertexBuffer(unsigned int vertexArrayId, size_t startPos, size_t vertexCounts);
	void DrawIndexBuffer(unsigned int vertexArrayId, size_t indexCount);

	bool BeginRender();
	void EndRender();

	float GetRenderTargetWidth() const;
	float GetRenderTargetHeight() const;
private:
	void OutputLinkErrorMessage(OpenGL& gl, unsigned int programId);
	void OutputShaderErrorMessage(OpenGL& gl, unsigned int shaderId, char* shaderFilename);
	char* LoadShaderSourceFile(const char* filename);
private:
	OpenGL* pGL;
	//PhongShader* pShader;
	HWND hWnd;
	DrawMode drawMode;
};

