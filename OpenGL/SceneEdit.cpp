#include "SceneEdit.h"
#include "RayCast.h"

SceneEdit::SceneEdit(class Scene& _scene)
	: scene(_scene)
{
	picker.SetNear(scene.GetCamera().GetNear());
	picker.SetFar(scene.GetCamera().GetFar());
}

SceneEdit::~SceneEdit() {

}

bool SceneEdit::PickObject(const Ray& _ray) {
	if (!_ray.IsValid()) {
		assert(0);
		return false;
	}

	picker.SetRay(_ray);

	if (!picker.HitTest(scene)) {
		picker.Clear();
		scene.Gizmo.Detach();

		return false;
	}


	//Show Gizmo
	//..
	const GameObjectPicker::PickedList& pickedObjects = picker.GetPickedList();
	scene.Gizmo.Attach(*pickedObjects.front().pObject);

	return true;
}

void SceneEdit::GetTransform(Vec4f& transform) {

}
//Vec3f SceneEdit::CalcDragOffsetInWorld(const Point2d& prev, const Point2d& cur) {
//
//}
void SceneEdit::MoveSelectedObject(GameObject& selection, const Vec3f& offset) {
	
}

void SceneEdit::MoveSelectedObject(GameObject& selection, const Vec3f& offset, const Vec3f& distance) {

}