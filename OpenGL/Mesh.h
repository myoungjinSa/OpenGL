#pragma once
#include <vector>
#include <array>
#include <list>
#include "Types.h"
#include "Compositor.h"
#include "RGBA.h"

class Renderer; 
class Shader;
class OpenGL;
class Mesh;
//the master Vertex. this is a superset of all possible vertex data.
struct VertexMaster {
	Vec3f position;
	RGBA color;
	Vec2f uv0;
	Vec3f normal;
	//Vec3f tangent;
	//Vec2f uv1;
	//Vec3f bitangent;
};
typedef unsigned char byte;
typedef void (VertexCopyCallback)(const VertexMaster& source, byte* destination);
typedef void (VertexBufferBindCallback)(OpenGL& gl, void* pVertexBuffer, unsigned int vertexBufferId, unsigned int vertexCount, unsigned int sizeofVertex);

byte* ReadBufferData(void* pBuffer, size_t targetDataSize);

struct Vertex final{
public:
	Vertex();
	Vertex(const Vec3f& pos, const Vec2f& uv, const Vec3f& normal, const RGBA& color);

	static void Copy(const VertexMaster& source, byte* pDestination);
	static void BindVertexBuffer(OpenGL& gl, void* pBuffer, unsigned int vertexBufferId, unsigned int vertexCount, unsigned int sizeOfVertex);

	Vec3f position;
	RGBA  color;
	Vec2f uv0;
	Vec3f normal;
};


struct Triangle{
public:
	Triangle();
	Triangle(const Vertex& v0, const Vertex& v1, const Vertex& v2);
	~Triangle();

	void Set(const Vertex& v0, const Vertex& v1, const Vertex& v2);
	Vertex* Get(size_t index);

	void SetPosition(size_t index, float posX, float posY, float posZ);
	void SetPosition(size_t index, const Vec3f& pos);
	Vec3f GetPosition(size_t index) const;

	void SetNormal(size_t index, float normalX, float normalY, float normalZ);
	void SetNormal(size_t index, const Vec3f& normal);
	Vec3f GetNormal(size_t index) const;

	void SetUV(size_t index, float u, float v);
	void SetUV(size_t index, const Vec2f& uv);
	Vec2f GetUV(size_t index) const;

	std::array<Vertex, 3> vertices;
};

class Mesh
{
	typedef unsigned int GLuint;
public:
	enum class TriangleType : unsigned char {
		MeshType_List,
		MeshType_Strip
	};


	Mesh();
	Mesh(Mesh&& other) = delete;
	Mesh(const Mesh& other) = delete;
	virtual ~Mesh();

	bool DoesHaveIndexBuffer() const { return 0 < indexCount; }
	bool Initialize(Renderer& renderer, VertexBufferBindCallback* pBindFuction, void* vertexData, unsigned int numVertices, unsigned int sizeofVertex, unsigned int* indexData, unsigned int numIndices);
	void BindToVAO(const OpenGL& gl, Shader* pShader,GLuint vaoID);
	void Shutdown(Renderer& renderer);
	void Render(Renderer& renderer);

	bool BuildVertexList(void* vertexData);
	bool BuildIndexList(unsigned int* indicesData);
	bool BuildTriangles(TriangleType triangleType);

	Mesh& operator=(const Mesh& other) = delete;
	Mesh& operator=(Mesh&& other) = delete;

	size_t	GetTriangleMeshCount() const;
	const Triangle& GetTriangleMesh(size_t index) const;
protected:
	std::vector<Vertex*> vertexList;
	std::vector<unsigned int> indexList;

	std::vector<Triangle> triangles;
	int vertexCount, indexCount;
	unsigned int vertexArrayId, vertexBufferId, indexBufferId;
};

class TerrainHeightImage;
class MeshBuilder {
public:
	enum MeshDataFlag : unsigned char {
		POSITION_BIT = 0,
		COLOR_BIT,
		UV0_BIT,
		UV1_BIT,
		NORMAL_BIT,
		TANGENT_BIT,
		BITANGENT_BIT,
		NORMALIZED_GLYPH_POSITION_BIT,
		NORMALIZED_STRING_POSITION_BIT,
		NORMALIZED_FRAG_POSITION_BIT,
		BONE_WEIGHTS_BIT,
		BONE_INDICES_BIT,
		NUM_MESH_DATA
	};

	struct PlaneData {
		PlaneData() {};
		PlaneData(const Vec3f& _initialPosition, const Vec3f& _right, const Vec3f& _up)
			: initialPosition(_initialPosition), right(_right), up(_up) {}
		Vec3f initialPosition;
		Vec3f right;
		Vec3f up;
	};

	MeshBuilder();

	//SETTERS///////////////////////////////////////////////////////////////////
	inline void SetColor(const RGBA& color) { stamp.color = color; SetMaskBit(COLOR_BIT); };

	inline void SetPosition(const Vec3f& pos) { stamp.position = pos; SetMaskBit(POSITION_BIT); }
	inline void SetPosition(float x, float y, float z) { stamp.position = Vec3f(x, y, z); SetMaskBit(POSITION_BIT); }
	inline void SetUV(const Vec2f& uv) { stamp.uv0 = uv; SetMaskBit(UV0_BIT); };
	inline void SetUV(float u, float v) { stamp.uv0 = Vec2f(u, v); SetMaskBit(UV0_BIT); };
	inline void SetNormal(const Vec3f& normal) { stamp.normal = normal, SetMaskBit(NORMAL_BIT); };
	inline void SetNormal(float nx, float ny, float nz) { stamp.normal = Vec3f(nx, ny, nz), SetMaskBit(NORMAL_BIT); }
	//inline void SetTangent(const Vec3f& tangent) { stamp.tangent = tangent, SetMaskBit(TANGENT_BIT); }
	//inline void SetTangent(float tx, float ty, float tz) { stamp.tangent = Vec3f(tx, ty, tz), SetMaskBit(TANGENT_BIT); }
	inline void SetMaskBit(const MeshDataFlag flag) {
		dataMask |= (1 << flag);
	};

	//QUERIES//////////////////////////////////////////////////////////////////////////
	inline bool IsInMask(const MeshDataFlag flag) { return ((dataMask & (1 << flag)) != 0); };
	void Begin();
	void End();

	void CopyToMesh(Renderer& renderers, Mesh& mesh, VertexBufferBindCallback* bindFunction, VertexCopyCallback* copyFunction, unsigned int sizeofVertex, Mesh::TriangleType triangleType = Mesh::TriangleType::MeshType_List);

	void AddGrid(int xStart, int zStart, int width, int length, const Vec3f& positionOffset, const Vec3f& scale, const RGBA& color, const TerrainHeightImage& HeightContext);
	void AddCylinder(const Vec3f& axis, const Vec3f& arm1, const Vec3f& arm2, uint32_t slices, const RGBA& color);
	void AddSphere(const Vec3f& position, float radius, int sectorCount, int stackCount);
	void AddCube(float sideLength, const RGBA& color);
	void AddCube(const Vec3f& center, const Vec3f& extent, const RGBA& color, bool bLeftHand = true);
	void AddQuad(const Vec3f& bottomLeft, const Vec3f& up, float upLength, const Vec3f& right, float rightLength, const Vec3f& normal, const RGBA& color = RGBA::WHITE, const Vec2f& uvOffset = Vec2f::ZERO, float uvStepSize = 1.0f);
	void AddQuad(const Vec3f& axis, const Rect2f& rect, const RGBA& color = RGBA::WHITE, const Vec2f& uvOffset = Vec2f::ZERO, float uvStepSize = 1.0f);
	void AddQuad(const Vec3f& center, const Vec3f& extent, const Vec3f& normal, const RGBA& color = RGBA::WHITE, const Vec2f& uvOffset = Vec2f::ZERO, float uvStepSize = 1.0f, bool bReversed = false);
	void AddLathe(const Vec3f& axis, const Vec3f& arm1, const Vec3f& arm2, int slices, const std::vector<Point2f>& points, const RGBA& color, const float epsilon = 0.0f);
	void AddXAxisCircle(float outerRadius, float innerRadius, const RGBA& color);
	void AddYAxisCircle(float outerRadius, float innerRadius, const RGBA& color);
	void AddZAxisCircle(float outerRadius, float innerRadius, const RGBA& color);
	void AddXAxisCone(const Vec3f& centerOffset, float halfWidth, float halfHegiht, float halfDepth, double angleStep, const RGBA& color);
	void AddYAxisCone(const Vec3f& centerOffset, float halfWidth, float halfHegiht, float halfDepth, double angleStep, const RGBA& color);
	void AddZAxisCone(const Vec3f& centerOffset, float halfWidth, float halfHegiht, float halfDepth, double angleStep, const RGBA& color);

	void AddVertex(const Vec3f& position);
	void AddIndex(int index);

	void SetMeshType(Mesh::TriangleType triangleType);

	void ComputeNormals();

	std::vector<VertexMaster> vertices;
	std::vector<unsigned int> indices;
	uint32_t dataMask;
private:
	VertexMaster stamp;
	unsigned int startIndex;
};


