#pragma once
#include <map>
#include <memory>
#include <functional>
#include "Matrix.h"
#include "Types.h"
#include "Mesh.h"
#include "RGBA.h"
#include "Object.h"



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
class Gizmos : public GameObject
{
public:
	enum class eTransformMode {
		TRANSLATE,
		ROTATE,
		SCALE
	};
	
	Gizmos();
	
	class GizmoImpl {
	public:
		enum class eInteract {
			NONE,
			TRANSLATE_X, TRANSLATE_Y, TRANSLATE_Z,
			TRANSLATE_YZ, TRANSLATE_XZ, TRANSLATE_XY,
			TRANSLATE_XYZ,
			ROTATE_X, ROTATE_Y, ROTATE_Z,
			SCALE_X, SCALE_Y, SCALE_Z,
			SCALE_XYZ
		};

		struct InteractionState {
			bool active{ false };
			bool hover{ false };
			Vec3f originalPosition;
			Vec4f originalOrientation;
			Vec3f originalScale;
			Vec3f clickOffset;
			eInteract interationMode;
		};

		struct GizmoMeshComponent {
			std::shared_ptr<Mesh> pGizmoMesh;
			RGBA baseColor;
			RGBA highlightColor;

			GizmoMeshComponent() = default;
			GizmoMeshComponent(std::function<std::shared_ptr<Mesh>()> meshBuildFunction, const RGBA& _baseColor, const RGBA& _highlightColor);
			GizmoMeshComponent(const GizmoMeshComponent& other);

			const GizmoMeshComponent& operator=(const GizmoMeshComponent& other);

		};

		GizmoImpl(Gizmos* pOwner, Renderer& renderer);
		
		bool Intersect(const Ray& ray, double& distance);
		void Update(float deltaTime);
		void Render(Renderer& renderer, const GizmoParameter& parameter, Camera* pCamera, const Scene& scene);

		std::shared_ptr<ColorShader> defaultShader;
		std::map<eInteract, GizmoMeshComponent> meshComponents;
		InteractionState interactionState;
		bool localToggle;
		bool hasClicked;					 // State to describe if the user has pressed the left mouse button during the last frame
		bool hasReleased;				     // State to describe if the user has released the left mouse button during the last frame

		Gizmos* pOwner;
	private:
		bool IntersectTranslationGizmos(const Ray& ray, double& distance);
		bool IntersectRotationGizmos(const Ray& ray, double& distance);
		bool IntersectScaleGizmos(const Ray& ray, double& distance);
	};
	bool Initialize(Renderer& renderer);

	void Attach(GameObject& Target);
	void Detach();

	void Update(float deltaTime);
	void Render(Renderer& renderer, Camera* pCamera, const Scene& scene);

	bool Intersect(const Ray& ray, double& distance);

	bool IsAlreadyAttached() const;
	const GameObject& GetAttachedObjects(uint32_t index) const;

	float ScaleByDistanceToTarget(const Vec3f& targetPos, float yfov, float pixelScale) const;
	eTransformMode GetMode() const;

	static bool Transform(const String& name, Gizmos& gizmo, RigidTransform& tr);
	
private:
	eTransformMode transformMode;
	std::unique_ptr<GizmoImpl> impl;
	std::vector<GameObject*> targets;
};

