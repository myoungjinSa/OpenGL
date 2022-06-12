#pragma once
#include "Drag.h"
#include "Scene.h"
#include "Types.h"
#include "Observer.h"

class Scene;
class GameObjectPicker;
class Drag;
class Ray;

class SceneEdit : public Observer{
public:
	enum class eDragMode {
		DRAG_MODE_NONE,
		DRAG_MODE_MOVING,
		DRAG_MODE_SIZING,
		DRAG_MODE_ROTATING,
	};

	class ObjectMemento {
	public:
		ObjectMemento() = default;

		void Set(const GameObject& object);
		void Restore(GameObject& object);

		Vec3f position;
	};


	SceneEdit(Scene& _scene);
	~SceneEdit();

	void DoFocus(const Point2i& pt);
	void DoDrag(eDragMode dragMode);

	bool PickObject(const Ray& ray);

	void MoveSelectedObject(GameObjects& selection);
	void RotateSelectedObject(GameObjects& selection);

	void ProcessEvent(Event& e);
private:
	GameObjectPicker picker;
	Scene& scene;
	Drag drag;
	eDragMode dragMode;
	DragContext dragContext;
	ObjectMemento objMemento;

	Quaternion CalcDragRotation(const GameObject& baseObject, const Point2i& prev, const Point2i& cur);
	Vec3f CalcDragOffsetInWorld(const GameObject& baseObject, const Point2i& prev, const Point2i& cur);
	Vec3f ClampDragOffset(Gizmos::GizmoHandle::eHandle handleType, const Vec3f& offset);
};