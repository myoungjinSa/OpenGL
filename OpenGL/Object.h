#pragma once
#include <memory>
#include <vector>
#include <list>
#include "Types.h"
#include "Component.h"
#include "Transform.h"
#include "Mesh.h"

void MakeWorldViewMatrix(const Matrix<float, 4, 4>& worldMatrix, const Matrix<float, 4, 4>& viewMatrix, Matrix<float, 4, 4>& modelViewMatrix);
void MakeNormalMatrix(const Matrix<float, 4, 4>& worldMatrix, Matrix<float, 3, 3>& normalMatrix);

class Material;
class Texture;
class CubemapTexture;
class Renderer;
class Ray;
class BoundingVolume;
class Renderer;
class Triangle;
class Shader;

class DirectionalLight;
class PointLight;
class SpotLight;

class Camera;
class ShaderParameter;


class Object {
public:
	class ObjectVisitor {
		virtual bool Visit(Object& obj) = 0;
	};

protected:
	template<typename T> std::shared_ptr<T> AddComponent() {
		static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");
		for (auto& component : components) {
			if (std::dynamic_pointer_cast<T>(component)) {
				return std::dynamic_pointer_cast<T>(component);
			}
		}

		std::shared_ptr<T> newComponent = std::make_shared<T>(this);
		components.push_back(newComponent);
		return newComponent;
	}
	template<typename T> std::shared_ptr<T> GetComponent() {
		static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");

		for (auto& component : components) {
			if (std::dynamic_pointer_cast<T>(component)) {
				return std::dynamic_pointer_cast<T>(component);
			}
		}
		return nullptr;
	}
	
	Object() {}
	virtual ~Object() {}
	
	std::vector<std::shared_ptr<Component>> components;
};

class ObjectFactory;
class GameObject : public Object
{
	friend class ObjectFactory;
public:
	std::shared_ptr<RigidTransform> transform;
	std::shared_ptr<Material> material;
	std::shared_ptr<BoundingVolume> boundingVolume;						
	std::shared_ptr<Shader> shader;						//��ü�� ���̴�
	
	explicit GameObject();
	virtual ~GameObject();
	
	GameObject(const GameObject& other) = delete;
	const GameObject& operator=(const GameObject& other) = delete;

	virtual bool Initialize(Renderer& renderer);
	virtual void Shutdown(Renderer& renderer) {}
	virtual void Render(Renderer& renderer) {}
	virtual void Render(Renderer& renderer, ShaderParameter& shaderParam);
	virtual void Update(float deltaTime){}

	virtual bool Intersect(const Ray& ray, double& distance);

	void SetPosition(float x, float y, float z);
	void SetPosition(const Vec3f& _position);
	Vec3f GetPosition() const;

	void SetOrientation(const Quaternion& q);
	Quaternion GetOrientation() const;

	void SetScale(float sx, float sy, float sz);
	void SetScale(const Vec3f& _scale);
	Vec3f GetScale() const;

	void SetLookRightUp(const Vec3f& look, const Vec3f& right, const Vec3f& up);
	Vec3f GetLook() const;
	Vec3f GetRight() const;
	Vec3f GetUp() const;

	void Move(const Vec3f& offset);
	void Move(const Vec3f& dir, float movingSpeed, float elapsedTime);
	
	void Rotate(float pitch, float yaw, float roll);
	void Rotate(const Quaternion& q);
	void Rotate(const Matrix<float, 3, 3>& rotationMatrix);

	void FillShaderParameter(ShaderParameter& shaderParam, const Matrix<float, 4, 4>& viewMatrix, const Matrix<float, 4, 4>& projectionMatrix, const DirectionalLight& directionalLight, const PointLight& pointLight, const SpotLight& spotLight, const Camera& Camera, int iObjNo);

	void GetWorldBoundingBox(Volumef& volume)const;
	void GetLocalBoundingBox(Volumef& volume)const;
	Matrix<float, 4, 4> GetWorldMatrix() const;

	Meshes& GetMeshes() { return meshes; }
protected:
	std::vector<std::shared_ptr<Component>> components;
	
	Meshes meshes;
	std::shared_ptr<Texture> diffuseMap;
	std::shared_ptr<Texture> normalMap;
	std::shared_ptr<CubemapTexture> environMap;
};

bool IntersectTriangle(const Ray& ray, const Vec3f& v0, const Vec3f& v1, const Vec3f& v2, double& distance);
bool IntersectTriangle(const Ray& ray, const Triangle& triangle, double& distance);
bool IntersectTriangle(const Ray& ray, const Vec3f& v0, const Vec3f& v1, const Vec3f& v2, bool bFrontOnly, double& distance);


class Cube : public GameObject {
public:
	Cube();
	Cube(const Vec3f& size);
	Cube(const Cube& other) = delete;
	Cube(Cube&& other) noexcept = delete;
	virtual ~Cube();

	bool Initialize(Renderer& renderer) override;
	void Shutdown(Renderer& renderer) override;
	void Render(Renderer& renderer, ShaderParameter& shaderParam) override;
	
	void Update(float deltaTime) override;

	Cube& operator=(const Cube& other) = delete;
	Cube& operator=(Cube&& other) noexcept = delete;
};

class Sphere : public GameObject {
public:
	explicit Sphere(float _radius, int _stackCount, int _sectorCount);
	Sphere(const Sphere& other) = delete;
	Sphere(Sphere&& other) noexcept = delete;
	~Sphere() override;

	bool Initialize(Renderer& renderer) override;
	void Shutdown(Renderer& renderer) override;
	void Render(Renderer& renderer, ShaderParameter& shaderParam) override;
	
	void Update(float deltaTime) override;

	Sphere& operator=(const Sphere& other) = delete;
	Sphere& operator=(Sphere&& other) noexcept = delete;
private:
	float radius;
	int stackCount;
	int sectorCount;
};


class Cubemap : public GameObject {
public:
	enum class eCubemapIndex {
		CUBEMAP_RIGHT,
		CUBEMAP_LEFT,
		CUBEMAP_TOP,
		CUBEMAP_BOTTOM,
		CUBEMAP_FRONT,
		CUBEMAP_BACK
	};

	explicit Cubemap();
	~Cubemap()override;

	bool Initialize(Renderer& renderer) override;
	void Shutdown(Renderer& renderer) override;
	void Render(Renderer& renderer, ShaderParameter& shaderParam) override;
};

class Cylinder : public GameObject {
public:
	explicit Cylinder(const Vec3f& _axis, const Vec3f& arm1, const Vec3f& arm2, uint32_t slices);
	Cylinder(const Cylinder& other) = delete;
	Cylinder(Cylinder&& other) noexcept = delete;
	~Cylinder() override;
	
	bool Initialize(Renderer& renderer) override;
	void Shutdown(Renderer& renderer) override;
	void Render(Renderer& renderer, ShaderParameter& shaderParam) override;
	
	void Update(float deltaTime) override;

	Cylinder& operator=(const Cylinder& other) = delete;
private:
	Vec3f axis;
	Vec3f arm1;
	Vec3f arm2;
	float sliceCount;
};

class GameObjects : public std::vector<GameObject*>
{
public:
	GameObjects();
	~GameObjects();

	void Add(GameObject& gameObj);
	bool Clear();
};

class PostProcessingShader;
class RenderTarget : public Object {
public:
	RenderTarget();
	~RenderTarget();

	bool Create(Renderer& renderer);
	void Render(Renderer& renderer, std::shared_ptr<PostProcessingShader>& shader);

	void SetSize(const Size2u& _size);
	void SetSize(size_t width, size_t height);
	Size2u GetSize() const;

	bool IsNull() const { return iFrameBuffer == 0; }

public:
	std::shared_ptr<Texture> colorTexture;
	std::shared_ptr<Texture> depthStencilTexture;
	unsigned int iFrameBuffer;
private:
	//std::shared_ptr<Shader> shader;
	std::shared_ptr<Mesh> mesh;
	Size2u size;
};

//class ObjectFactory {
//public:
//	enum class eObjectType {
//		OBJECT_CUBE,
//		OBJECT_SPHERE,
//		OBJECT_CYLINDER,
//	};
//	enum class eShaderType {
//		SHADER_COLOR,
//		SHADER_TEXTURE,
//		SHADER_PHONG,
//
//		SHADER_DEFAULT = SHADER_COLOR,
//	};
//
//	std::shared_ptr<GameObject>& CreateGameObject(Renderer& renderer, eObjectType objType, eShaderType shaderType = eShaderType::SHADER_DEFAULT);
//};
