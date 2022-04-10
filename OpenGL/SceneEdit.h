#pragma once
#include "Drag.h"
#include "Scene.h"
#include "Matrix.h"

class Scene;
class GameObjectPicker;
class Drag;
class Ray;

class SceneEdit {
public:
	SceneEdit(Scene& _scene);
	~SceneEdit();

	void GetTransform(Vec4f& transform);
	bool PickObject(const Ray& ray);
	void MoveSelectedObject(GameObject& Selection, const Vec3f& offset);
	void MoveSelectedObject(GameObject& Selection, const Vec3f& offset, const Vec3f& distance);
	//Vec3f CalcDragOffsetInWorld(const Point2d& prev, const Point2d& cur);
private:
	GameObjectPicker picker;
	Scene& scene;
	Drag drag;
	DragContext dragContext;
};