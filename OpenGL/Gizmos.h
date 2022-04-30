#pragma once
#include <map>
#include <memory>
#include <functional>
#include <list>
#include "Types.h"
#include "Mesh.h"
#include "RGBA.h"
#include "Object.h"
#include "BoundingVolume.h"

struct GizmoParameter {
	float snap_translation{ 0.f };      // World-scale units used for snapping translation
	float snap_scale{ 0.f };            // World-scale units used for snapping scale
	float snap_rotation{ 0.f };         // Radians used for snapping rotation quaternions (i.e. PI/8 or PI/16)
	
	Size2f viewport_size;				// 3d viewport used to render the view
	Vec3f ray_origin;				   // world-space ray origin (i.e. the camera position)
	Vec3f ray_direction;				// world-space ray direction

	std::vector<GameObject*> attachTargets;	//The target to attach.
};

class String;
class Camera;
class RigidTransform;
class Renderer;
class ColorShader;
class Scene;
class Ray;

class Gizmos final : public GameObject
{
public:
	enum class eTransformMode {
		TRANSLATE,
		ROTATE,
		SCALE
	};


	class GizmoHandle : public GameObject{
	public:
		enum class eHandle {
			NONE,
			TRANSLATE_X, TRANSLATE_Y, TRANSLATE_Z,
			TRANSLATE_YZ, TRANSLATE_XZ, TRANSLATE_XY,
			TRANSLATE_XYZ,
			ROTATE_X, ROTATE_Y, ROTATE_Z,
			SCALE_X, SCALE_Y, SCALE_Z,
			SCALE_XYZ
		};
		const Gizmos& owner;
		RGBA baseColor;
		eHandle type;

		GizmoHandle() = default;
		GizmoHandle(const Gizmos& owner, std::function<bool(Gizmos::GizmoHandle&, Renderer&)> _meshBuildFunction, const RGBA& _baseColor, const RGBA& _highlightColor, eHandle handleType);
		GizmoHandle(const GizmoHandle& other);

		bool Initialize(Renderer& renderer) override;
		bool Intersect(const Ray& ray, double& distance);
		void Render(Renderer& renderer, ShaderParameter& shaderParam) override;
		void Shutdown(Renderer& renderer) override;

		const GizmoHandle& operator=(const GizmoHandle& other);

	private:
		std::shared_ptr<ColorShader> defaultShader;
		std::function<bool(Gizmos::GizmoHandle&, Renderer&)> meshBuildFunction;
	};
	class GizmoContext {
	public:
		GizmoContext() {
			editingHandle = GizmoHandle::eHandle::NONE;
		}

		GizmoHandle::eHandle editingHandle;
	};

	class GizmoHandles : public std::map<GizmoHandle::eHandle, GizmoHandle> {
	public:
		bool Add(const GizmoHandle& handle);

		bool DoesExist(GizmoHandle::eHandle handleType) const;
		void Render(Renderer& renderer, ShaderParameter& shaderParam, eTransformMode transformMode) ;
	
		GizmoHandle::eHandle Intersect(const Ray& ray, double& distance, eTransformMode transformMode);
	};
	
	Gizmos();

	bool Initialize(Renderer& renderer);
#ifdef _DEBUG
	bool VerifyGizmos() const;
#endif
	bool Intersect(const Ray& ray, double& distance) override;
	void Attach(GameObject& Target);
	void Detach();

	void Focus(GizmoHandle::eHandle handleType);
	void Render(Renderer& renderer, Camera* pCamera, const Scene& scene);

	bool IsAlreadyAttached() const;
	const GameObject& GetAttachedObjects(uint32_t index) const;

	float ScaleByDistanceToTarget(const Vec3f& targetPos, float yfov, float pixelScale) const;
	eTransformMode GetMode() const;

	GizmoHandle::eHandle GetEditingHandle() const { return context.editingHandle; }
private:
	GizmoContext context;
	eTransformMode transformMode;
	GameObjects targets;

	GizmoHandles handles;

	Point2i oldMousePoint;
};

