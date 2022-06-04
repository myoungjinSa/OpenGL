#pragma once
#include "Common.h"
#include "Mesh.h"
#include "Types.h"
#include "Platform.h"
#include "String/String.h"

/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;

class GraphicDevice;
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
		LINE_STRIP,
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
	virtual ~Renderer();

	virtual bool Initialize(GraphicDevice* pDevice);
	virtual void Shutdown(unsigned int shaderProgram, unsigned int vertexShader, unsigned int fragmentShader);

	virtual unsigned int CreateShader() = 0;
	virtual bool CreateRenderTarget(RenderTarget& renderTarget, const Size2u& screenSize, bool downSamplingTarget) { return false; }
	virtual bool CreateTexture(Texture& texture, const Size2u& size, bool managed) { return false; }
	virtual void DeleteTexture(Texture& texture) {}

	virtual void SetShader(unsigned int shaderProgram) {}
	virtual bool CompileVertexShader(const char* vsFilename, unsigned int& vertexShader) { return false; }
	virtual bool CompileFragmentShader(const char* fsFilename, unsigned int& fragmentShader) {return false;}
	virtual bool CompileGeometryShader(const char* gsFilename, unsigned int& geometryShader) {return false;}
	virtual bool BindVertexAttrib(unsigned int shaderProgram, unsigned int vertexShader, unsigned int fragmentShader, int vertexArgs, ...) { return false; }
	
	virtual void SetWindingOrder(WindingOrder order) {}
	virtual void SetCullingMode(CullingMode cullMode) {}
	virtual void SetDepthTest(bool bEnable) {}
	Renderer::DrawMode GetDrawMode() const { return drawMode; }
	void SetDrawMode(Renderer::DrawMode drawMode);
	void SetRenderMode(RenderMode renderMode);
	Renderer::RenderMode GetRenderMode() const { return renderMode; }
	
	virtual bool SetShaderParameter(unsigned int shaderProgram, const Vec4f& vec4, String variableName) { return false; }
	virtual bool SetShaderParameter(unsigned int shaderProgram, const Vec3f& vec3, String variableName) { return false; }
	
	virtual bool SetShaderParameter(unsigned int shaderProgram, Vec4f& vec4, String variableName) { return false; }
	virtual bool SetShaderParameter(unsigned int shaderProgram, Vec3f& vec3, String variableName) { return false; }
	virtual bool SetShaderParameter(unsigned int shaderProgram, int integer, String variableName) { return false; }
	
	virtual bool SetShaderParameter(unsigned int shaderProgram, Matrix<float, 4, 4>& matrix, String variableName) { return false; }
	virtual bool SetShaderParameter(unsigned int shaderProgram, const Matrix<float, 4, 4>& matrix, String variableName) { return false; }

	virtual bool AllocateVertexBuffer(unsigned int& vertexArrayId, unsigned int& vertexBufferId, void* vertexData, VertexBufferBindCallback* pBindFunction, unsigned int numVertices, unsigned int sizeofVertex) { return false; }
	virtual bool AllocateIndexBuffer(unsigned int& indexBufferId, size_t indexCount, unsigned int* indexData) { return false; }
	virtual bool AllocateFrameBuffer(unsigned int& colorBuffer, unsigned int& depthBuffer, unsigned int& fbo) { return false; }
	virtual bool AllocateTextures(unsigned int& textureID, unsigned int textureCount) { return false; }
	
	virtual void BindTexture(unsigned int textureId) {}
	virtual void BindCubemapTexture(unsigned int textureId){}
	virtual void SetImage(unsigned int target, void* pImage, unsigned int width, unsigned int height) {}
	virtual void SetSampleMode(bool bCubemap = false){}
	virtual void SetFiltering(bool bCubemap = false){}

	virtual void SetDepthFunc(unsigned int Func) {}
	virtual void EnableCulling(bool bEnable = true) {}
	virtual void DisableVertexAttribArray(size_t vertexAttribCount){}
	virtual void ReleaseVertexBuffers(unsigned int& vertexBufferId, size_t bufferCount){}
	virtual void ReleaseIndexBuffers(unsigned int& vertexBufferId, size_t bufferCount){}
	virtual void ReleaseVertexArray(unsigned int& vertedxArrayId, size_t arrayCount){}
	virtual void DrawVertexBuffer(unsigned int vertexArrayId, size_t startPos, size_t vertexCounts){}
	virtual void DrawIndexBuffer(unsigned int vertexArrayId, size_t indexCount){}

	virtual bool BeginRender() { return false; }
	virtual void EndRender() {}
	
	const Size2u& GetScreenSize() const { return screenSize; }
protected:
	HWND hWnd;
	DrawMode drawMode;
	RenderMode renderMode;

	Size2u  screenSize;

	void OutputLinkErrorMessage(GraphicDevice& device, unsigned int programId);
	void OutputShaderErrorMessage(GraphicDevice& device, unsigned int shaderId, char* shaderFilename);
	char* LoadShaderSourceFile(const char* filename);
};



class OpenGLRenderer : public Renderer {
public:
	~OpenGLRenderer() override;
	bool Initialize(GraphicDevice* pDevice) override;
	void Shutdown(unsigned int shaderProgram, unsigned int vertexShader, unsigned int fragmentShader) override;

	unsigned int CreateShader() override;
	bool CreateRenderTarget(RenderTarget& renderTarget, const Size2u& screenSize, bool downSampleingTarget);
	bool CreateTexture(Texture& texture, const Size2u& size, bool managed)override;
	void DeleteTexture(Texture& texture)override;

	void SetShader(unsigned int shaderProgram) override;
	bool CompileVertexShader(const char* vsFilename, unsigned int& vertexShader) override;
	bool CompileFragmentShader(const char* fsFilename, unsigned int& fragmentShader)override;
	bool CompileGeometryShader(const char* gsFilename, unsigned int& geometryShader)override;
	bool BindVertexAttrib(unsigned int shaderProgram, unsigned int vertexShader, unsigned int fragmentShader, int vertexArgs, ...) override;

	void SetWindingOrder(Renderer::WindingOrder order) override;
	void SetCullingMode(Renderer::CullingMode cullMode)override;
	void SetDepthTest(bool bEnable) override;

	bool SetShaderParameter(unsigned int shaderProgram, const Vec4f& vec4, String variableName) override;
	bool SetShaderParameter(unsigned int shaderProgram, const Vec3f& vec3, String variableName) override;

	bool SetShaderParameter(unsigned int shaderProgram, Vec4f& vec4, String variableName)override; 
	bool SetShaderParameter(unsigned int shaderProgram, Vec3f& vec3, String variableName)override;
	bool SetShaderParameter(unsigned int shaderProgram, int integer, String variableName)override;

	bool SetShaderParameter(unsigned int shaderProgram, Matrix<float, 4, 4>& matrix, String variableName)override;
	bool SetShaderParameter(unsigned int shaderProgram, const Matrix<float, 4, 4>& matrix, String variableName) override;

	bool AllocateVertexBuffer(unsigned int& vertexArrayId, unsigned int& vertexBufferId, void* vertexData, VertexBufferBindCallback* pBindFunction, unsigned int numVertices, unsigned int sizeofVertex) override;
	bool AllocateIndexBuffer(unsigned int& indexBufferId, size_t indexCount, unsigned int* indexData) override;
	bool AllocateFrameBuffer(unsigned int& colorBuffer, unsigned int& depthBuffer, unsigned int& fbo) override;
	bool AllocateTextures(unsigned int& textureID, unsigned int textureCount) override;

	void BindTexture(unsigned int textureId)override;
	void BindCubemapTexture(unsigned int textureId)override;
	void SetImage(unsigned int target, void* pImage, unsigned int width, unsigned int height) override;
	void SetSampleMode(bool bCubemap = false)override;
	void SetFiltering(bool bCubemap = false)override;

	void SetDepthFunc(unsigned int Func) override;
	void EnableCulling(bool bEnable = true)override;
	void DisableVertexAttribArray(size_t vertexAttribCount)override;
	void ReleaseVertexBuffers(unsigned int& vertexBufferId, size_t bufferCount)override;
	void ReleaseIndexBuffers(unsigned int& vertexBufferId, size_t bufferCount)override;
	void ReleaseVertexArray(unsigned int& vertedxArrayId, size_t arrayCount)override;
	void DrawVertexBuffer(unsigned int vertexArrayId, size_t startPos, size_t vertexCounts)override;
	void DrawIndexBuffer(unsigned int vertexArrayId, size_t indexCount)override;

	bool BeginRender() override;
	void EndRender() override;
protected:
	OpenGL* pDevice;
};