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


enum class eCompare : unsigned int {
	ALWAYS,			
	NEVER,			
	LESS,		
	LEQUAL,
	EQUAL,
	NOTEQUAL,
	GREATER,		
	GEQUAL
};

enum class eFace : unsigned int {
	Back,
	Front,
	FrontAndBack
};


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


	enum class StencilOp : unsigned int {
		KEEP,
		ZERO,
		REPLACE,
		INCREMENT,
		DECREMENT,
		INVERT,
		INCREMENT_WRAP,
		DECREMENT_WRAP
	};


	Renderer();
	Renderer(const Renderer&) = delete;
	virtual ~Renderer();

	virtual bool Initialize(GraphicDevice* pDevice);
	virtual void Shutdown(unsigned int shaderProgram, unsigned int vertexShader, unsigned int fragmentShader);

	virtual unsigned int CreateShader() = 0;
	virtual bool CreateRenderTarget(RenderTarget& renderTarget, const Size2u& screenSize, bool downSamplingTarget) { return false; }
	virtual void DestroyRenderTarget(RenderTarget& renderTarget) {}
	virtual bool CreateTexture(Texture& texture, const Size2u& size, void* pImage) { return false; }
	virtual bool CreateDepthStencilTexture(Texture& texture, const Size2u& size) { return true; }
	virtual void DeleteTexture(Texture& texture) {}

	virtual void SetShader(unsigned int shaderProgram) {}
	virtual bool CompileVertexShader(const char* vsFilename, unsigned int& vertexShader) { return false; }
	virtual bool CompileFragmentShader(const char* fsFilename, unsigned int& fragmentShader) {return false;}
	virtual bool CompileGeometryShader(const char* gsFilename, unsigned int& geometryShader) {return false;}
	virtual bool BindVertexAttrib(unsigned int shaderProgram, unsigned int vertexShader, unsigned int fragmentShader, int vertexArgs, ...) { return false; }
	virtual bool BindVertexAttrib(unsigned int shaderProgram, unsigned int vertexShader, unsigned int geometryShader, unsigned int fragmentShader, int vertexArgs, ...) { return false; }

	virtual void SetViewport(const Viewporti& viewport) {};
	virtual void SetWindingOrder(WindingOrder order) {}
	virtual void SetCullFace(eFace face) {}
	virtual void EnableDepthTest(bool bEnable) {}
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
	virtual bool SetShaderParameter(unsigned int shaderProgram, Matrix<float, 3, 3>& matrix, String variableName) { return false; }
	virtual bool SetShaderParameter(unsigned int shaderProgram, const Matrix<float, 3, 3>& matrix, String variableName) { return false; }


	virtual bool AllocateVertexBuffer(unsigned int& vertexArrayId, unsigned int& vertexBufferId, void* vertexData, VertexBufferBindCallback* pBindFunction, unsigned int numVertices, unsigned int sizeofVertex) { return false; }
	virtual bool AllocateIndexBuffer(unsigned int& indexBufferId, size_t indexCount, unsigned int* indexData) { return false; }
	virtual bool AllocateFrameBuffer(unsigned int& colorBuffer, unsigned int& depthBuffer, unsigned int& fbo) { return false; }
	virtual bool AllocateTextures(unsigned int& textureID, unsigned int textureCount) { return false; }
	
	virtual void BindTexture(unsigned int textureId) {}
	virtual void BindCubemapTexture(unsigned int textureId){}
	virtual void SetImage(unsigned int target, void* pImage, unsigned int width, unsigned int height, unsigned int textureType, unsigned int realType, unsigned int dataType) {}
	virtual void SetSampleMode(bool bCubemap, int filterMode, int wrappingMode){}

	virtual void EnableStencilTest(bool bEnable) {}
	virtual void SetStencilFunc(int face, eCompare _stencilFunc, int ref, unsigned int mask){}
	virtual void OperateAfterStencilTest(int face, StencilOp stencilFailed, StencilOp depthFailed, StencilOp depthPassed) {}
	virtual void SetDepthFunc(eCompare depthFunc) {}
	virtual void EnableCulling(bool bEnable = true) {}
	virtual void DisableVertexAttribArray(size_t vertexAttribCount){}
	virtual void ReleaseVertexBuffers(unsigned int& vertexBufferId, size_t bufferCount){}
	virtual void ReleaseIndexBuffers(unsigned int& vertexBufferId, size_t bufferCount){}
	virtual void ReleaseVertexArray(unsigned int& vertedxArrayId, size_t arrayCount){}
	virtual void DrawVertexBuffer(unsigned int vertexArrayId, size_t startPos, size_t vertexCounts){}
	virtual void DrawIndexBuffer(unsigned int vertexArrayId, size_t indexCount){}
	virtual void BindRenderTarget(RenderTarget& renderTarget) {}
	virtual void UnbindRenderTarget(RenderTarget& renderTarget) {}
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

	virtual int GetFace(eFace face) const { return 0; }
	virtual int GetDepthCompare(eCompare depthCmp) const { return 0; }
	virtual int GetStencilOp(StencilOp stencilOp) const{ return 0; }
};



class OpenGLRenderer : public Renderer {
public:
	~OpenGLRenderer() override;
	bool Initialize(GraphicDevice* pDevice) override;
	void Shutdown(unsigned int shaderProgram, unsigned int vertexShader, unsigned int fragmentShader) override;

	unsigned int CreateShader() override;
	bool CreateRenderTarget(RenderTarget& renderTarget, const Size2u& screenSize, bool downSampleingTarget);
	void DestroyRenderTarget(RenderTarget& renderTarget) override;
	bool CreateTexture(Texture& texture, const Size2u& size, void* pImage)override;
	bool CreateDepthStencilTexture(Texture& texture, const Size2u& size)override;
	void DeleteTexture(Texture& texture)override;

	void SetShader(unsigned int shaderProgram) override;
	bool CompileVertexShader(const char* vsFilename, unsigned int& vertexShader) override;
	bool CompileFragmentShader(const char* fsFilename, unsigned int& fragmentShader)override;
	bool CompileGeometryShader(const char* gsFilename, unsigned int& geometryShader)override;
	bool BindVertexAttrib(unsigned int shaderProgram, unsigned int vertexShader, unsigned int fragmentShader, int vertexArgs, ...) override;
	bool BindVertexAttrib(unsigned int shaderProgram, unsigned int vertexShader, unsigned int geometryShader, unsigned int fragmentShader, int vertexArgs, ...) override;

	void SetViewport(const Viewporti& viewport)override;
	void SetWindingOrder(Renderer::WindingOrder order) override;
	void SetCullFace(eFace cullMode)override;
	void EnableDepthTest(bool bEnable) override;

	bool SetShaderParameter(unsigned int shaderProgram, const Vec4f& vec4, String variableName) override;
	bool SetShaderParameter(unsigned int shaderProgram, const Vec3f& vec3, String variableName) override;

	bool SetShaderParameter(unsigned int shaderProgram, Vec4f& vec4, String variableName)override; 
	bool SetShaderParameter(unsigned int shaderProgram, Vec3f& vec3, String variableName)override;
	bool SetShaderParameter(unsigned int shaderProgram, int integer, String variableName)override;

	bool SetShaderParameter(unsigned int shaderProgram, Matrix<float, 4, 4>& matrix, String variableName)override;
	bool SetShaderParameter(unsigned int shaderProgram, const Matrix<float, 4, 4>& matrix, String variableName) override;
	bool SetShaderParameter(unsigned int shaderProgram, Matrix<float, 3, 3>& matrix, String variableName)override;
	bool SetShaderParameter(unsigned int shaderProgram, const Matrix<float, 3, 3>& matrix, String variableName) override;

	bool AllocateVertexBuffer(unsigned int& vertexArrayId, unsigned int& vertexBufferId, void* vertexData, VertexBufferBindCallback* pBindFunction, unsigned int numVertices, unsigned int sizeofVertex) override;
	bool AllocateIndexBuffer(unsigned int& indexBufferId, size_t indexCount, unsigned int* indexData) override;
	bool AllocateFrameBuffer(unsigned int& colorBuffer, unsigned int& depthBuffer, unsigned int& fbo) override;
	bool AllocateTextures(unsigned int& textureID, unsigned int textureCount) override;

	void BindTexture(unsigned int textureId)override;
	void BindCubemapTexture(unsigned int textureId)override;
	void SetImage(unsigned int target, void* pImage, unsigned int width, unsigned int height, unsigned int textureType, unsigned int realType, unsigned int dataType) override;
	void SetSampleMode(bool bCubemap, int filterMode, int wrappingMode);


	void EnableStencilTest(bool bEnable)override;
	void SetStencilFunc(int face, eCompare _stencilFunc, int ref, unsigned int mask)override;
	void OperateAfterStencilTest(int face, StencilOp stencilFailed, StencilOp depthFailed, StencilOp depthPassed)override;
	void SetDepthFunc(eCompare depthFunc) override;
	void EnableCulling(bool bEnable = true)override;
	void DisableVertexAttribArray(size_t vertexAttribCount)override;
	void ReleaseVertexBuffers(unsigned int& vertexBufferId, size_t bufferCount)override;
	void ReleaseIndexBuffers(unsigned int& vertexBufferId, size_t bufferCount)override;
	void ReleaseVertexArray(unsigned int& vertedxArrayId, size_t arrayCount)override;
	void DrawVertexBuffer(unsigned int vertexArrayId, size_t startPos, size_t vertexCounts)override;
	void DrawIndexBuffer(unsigned int vertexArrayId, size_t indexCount)override;
	void BindRenderTarget(RenderTarget& renderTarget)override;
	void UnbindRenderTarget(RenderTarget& renderTarget)override;

	bool BeginRender() override;
	void EndRender() override;
protected:
	OpenGL* pDevice;

	int GetFace(eFace face) const override;
	int GetDepthCompare(eCompare depthCmp) const override;
	int GetStencilOp(StencilOp stencilOp)const override;
};