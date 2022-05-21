#pragma once
#include "Common.h"
#include "OpenGL.h"
#include "Mesh.h"
#include "Types.h"

/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;

class OpenGL;
class Object;
class PhongShader;
class RenderTarget;
class ColorShader;
class Camera;
class String;
class Texture;
template<typename> class Size;
class Renderer
{
public:
	enum class DrawMode : unsigned int {
		POINTS,
		LINES,
		TRIANGLES,
		TRIANGLE_STRIP,
		NUM_DRAW_MODES
	};

	enum class RenderMode : unsigned int {
		SOLID,
		WIREFRAME
	};

	enum class WindingOrder : unsigned int {
		CW,
		CCW
	};

	enum class CullingMode : unsigned int {
		Back,
		Front,
		FrontAndBack
	};

	Renderer();
	Renderer(const Renderer&) = delete;
	~Renderer();

	bool Initialize(OpenGL* pOpenGL); 
	void Shutdown(unsigned int shaderProgram, unsigned int vertexShader, unsigned int fragmentShader);

	unsigned int CreateShader();
	bool CreateRenderTarget(RenderTarget& renderTarget, const Size2u& screenSize, bool downSamplingTarget);
	bool CreateTexture(Texture& texture, const Size2u& size, bool managed);

	void SetShader(unsigned int shaderProgram);
	bool CompileVertexShader(const char* vsFilename, unsigned int& vertexShader);
	bool CompileFragmentShader(const char* fsFilename, unsigned int& fragmentShader);
	bool CompileGeometryShader(const char* gsFilename, unsigned int& geometryShader);
	bool BindVertexAttrib(unsigned int shaderProgram, unsigned int vertexShader, unsigned int fragmentShader, int vertexArgs, ...);
	
	void SetWindingOrder(WindingOrder order);
	void SetCullingMode(CullingMode cullMode);
	void SetDepthTest(bool bEnable);
	void SetDrawMode(Renderer::DrawMode drawMode);
	void SetRenderMode(RenderMode renderMode);
	Renderer::DrawMode GetDrawMode() const { return drawMode; }
	Renderer::RenderMode GetRenderMode() const { return renderMode; }
	
	bool SetShaderParameter(unsigned int shaderProgram, const Vec4f& vec4, String variableName);
	bool SetShaderParameter(unsigned int shaderProgram, const Vec3f& vec3, String variableName);

	bool SetShaderParameter(unsigned int shaderProgram, Vec4f& vec4, String variableName);
	bool SetShaderParameter(unsigned int shaderProgram, Vec3f& vec3, String variableName);
	bool SetShaderParameter(unsigned int shaderProgram, int integer, String variableName);

	bool SetShaderParameter(unsigned int shaderProgram, Matrix<float, 4, 4>& matrix, String variableName);
	bool SetShaderParameter(unsigned int shaderProgram, const Matrix<float, 4, 4>& matrix, String variableName);


	bool AllocateVertexBuffer(unsigned int& vertexArrayId, unsigned int& vertexBufferId, void* vertexData, VertexBufferBindCallback* pBindFunction, unsigned int numVertices, unsigned int sizeofVertex);
	bool AllocateIndexBuffer(unsigned int& indexBufferId, size_t indexCount, unsigned int* indexData);
	bool AllocateFrameBuffer(unsigned int& colorBuffer, unsigned int& depthBuffer, unsigned int& fbo);
	bool AllocateTextures(unsigned int& textureID, unsigned int textureCount);
	
	void BindTexture(unsigned int textureId);
	void BindCubemapTexture(unsigned int textureId);
	void SetImage(unsigned int target, void* pImage, unsigned int width, unsigned int height);
	void SetSampleMode(bool bCubemap = false);
	void SetFiltering(bool bCubemap = false);

	void SetDepthFunc(unsigned int Func);

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
	RenderMode renderMode;
};

