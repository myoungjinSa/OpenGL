#include "SceneEdit.h"
#include "RayCast.h"
#include "Input.h"

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
		MouseInput::Detach(this);

		return false;
	}


	//Show Gizmo
	//..
	const GameObjectPicker::PickedList& pickedObjects = picker.GetPickedList();
	scene.Gizmo.Attach(*pickedObjects.front().pObject);

	MouseInput::Attach(this);

	return true;
}

void SceneEdit::GetTransform(Vec4f& transform) {

}
//Vec3f SceneEdit::CalcDragOffsetInWorld(const Point2d& prev, const Point2d& cur) {
//
//}

Vec3f SceneEdit::CalcDragOffsetInWorld(const GameObject& baseObject, const Point2i& prev, const Point2i& cur) {
	Ray curRay = scene.GetRay(cur, scene.GetSceneSize());
	Ray prevRay = scene.GetRay(prev, scene.GetSceneSize());

	Volumef volume;
	baseObject.GetWorldBoundingBox(volume);

	Matrix<float, 4, 4> worldMatrix = Matrix<float, 4, 4>::Identity();
	
	Vec4f vertices[8];
	vertices[0] = Transform(worldMatrix, Vec4f(volume.min.x, volume.max.y, volume.max.z, 1.0f));
	vertices[1] = Transform(worldMatrix, Vec4f(volume.max.x, volume.max.y, volume.max.z, 1.0f));
	vertices[2] = Transform(worldMatrix, Vec4f(volume.max.x, volume.max.y, volume.min.z, 1.0f));
	vertices[3] = Transform(worldMatrix, Vec4f(volume.min.x, volume.max.y, volume.min.z, 1.0f));
	vertices[4] = Transform(worldMatrix, Vec4f(volume.min.x, volume.min.y, volume.max.z, 1.0f));
	vertices[5] = Transform(worldMatrix, Vec4f(volume.max.x, volume.min.y, volume.max.z, 1.0f));
	vertices[6] = Transform(worldMatrix, Vec4f(volume.max.x, volume.min.y, volume.min.z, 1.0f));
	vertices[7] = Transform(worldMatrix, Vec4f(volume.min.x, volume.min.y, volume.min.z, 1.0f));



	return Vec3f();
}
void SceneEdit::MoveSelectedObject(GameObjects& selection) {
	if (selection.empty())
		return;

	Vec3f offset = CalcDragOffsetInWorld(*selection.at(0), drag.GetBeginPoint(), drag.GetEndPoint());

}

void SceneEdit::MoveSelectedObject(GameObjects& selection, const Vec3f& offset) {
	
}

void SceneEdit::MoveSelectedObject(GameObjects& selection, const Vec3f& offset, const Vec3f& distance) {

}

void SceneEdit::ProcessEvent(Event& e) {
	if (MouseInput::MouseEvent* pMouseEvent = dynamic_cast<MouseInput::MouseEvent*>(&e)) {
		if (pMouseEvent->mouseState == MouseInput::MouseEvent::MOUSE_STATE::MOUSE_STATE_COUNT) {
			return;
		}

		Point2i newMousePoint = pMouseEvent->mousePoint;

		if (pMouseEvent->mouseState == MouseInput::MouseEvent::MOUSE_STATE::LBUTTON_DOWN) {
			
			drag.Begin(newMousePoint);
		}
		if (pMouseEvent->mouseState == MouseInput::MouseEvent::MOUSE_STATE::LBUTTON_UP) {
			drag.End();
		}

		if (pMouseEvent->mouseState == MouseInput::MouseEvent::MOUSE_STATE::LEFT_BUTTON_DRAG) {
			drag.Track(newMousePoint);

			if (drag.IsTracked()) {
				DoDrag(eDragMode::DRAG_MODE_MOVING);
			}
		}
	}
}

void SceneEdit::DoDrag(eDragMode dragMode) {
	if (dragMode == eDragMode::DRAG_MODE_MOVING) {
		GameObjects selectedObjects = picker.GetSelectedObjects();
		
		MoveSelectedObject(selectedObjects);
	}
}