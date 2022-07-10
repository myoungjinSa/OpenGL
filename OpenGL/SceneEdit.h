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
	enum eDragMode {
		DRAG_MODE_NONE,
		DRAG_MODE_TRANSLATE,
		DRAG_MODE_ROTATE,
		DRAG_MODE_SCALE,
	};
	class ObjectMemento {
	public:
		ObjectMemento() = default;

		void Set(const GameObject& object);
		void Restore(GameObject& object);

		Vec3f position;
		Quaternion orientation;
		Vec3f look;
		Vec3f up;
		Vec3f right;
	};


	SceneEdit(Scene& _scene);
	~SceneEdit();

	void DoFocus(const Point2i& pt);
	void DoDrag();

	bool PickObject(const Ray& ray);

	void MoveSelectedObject(GameObjects& selection);
	void RotateSelectedObject(GameObjects& selection);

	void ProcessEvent(Event& e);
private:
	GameObjectPicker picker;
	Scene& scene;
	Drag drag;
	DragContext dragContext;
	ObjectMemento objMemento;
	bool bWorldCoordMode;

	Matrix<float, 3, 3> CalcRotationMatrix(const GameObject& baseObject, const Vec3f& prevPickedPoint, const Point2i& cur);
	Quaternion CalcRotationQuaternion(const GameObject& baseObject, const Vec3f& prevPickedPoint, const Point2i& cur);
	Vec3f CalcDragOffsetInWorld(const GameObject& baseObject, const Point2i& prev, const Point2i& cur);
	Vec3f ClampDragOffset(Gizmos::GizmoHandle::eHandle handleType, const Vec3f& offset);
};