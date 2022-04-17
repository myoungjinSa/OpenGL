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
	};
	SceneEdit(Scene& _scene);
	~SceneEdit();

	void DoDrag(eDragMode dragMode);

	void GetTransform(Vec4f& transform);
	bool PickObject(const Ray& ray);

	void MoveSelectedObject(GameObjects& selection);
	void MoveSelectedObject(GameObjects& Selection, const Vec3f& offset);
	void MoveSelectedObject(GameObjects& Selection, const Vec3f& offset, const Vec3f& distance);

	void ProcessEvent(Event& e);
private:
	GameObjectPicker picker;
	Scene& scene;
	Drag drag;
	eDragMode dragMode;
	DragContext dragContext;
	
	Vec3f CalcDragOffsetInWorld(const GameObject& baseObject, const Point2i& prev, const Point2i& cur);
};