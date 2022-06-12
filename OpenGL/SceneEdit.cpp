#include "SceneEdit.h"
#include "RayCast.h"
#include "Input.h"
#include "Logger.h"

void SceneEdit::ObjectMemento::Set(const GameObject& gameObject) {
	position = gameObject.GetPosition();
}
void SceneEdit::ObjectMemento::Restore(GameObject& gameObject) {
	gameObject.SetPosition(position);
}


SceneEdit::SceneEdit(class Scene& _scene)
	: scene(_scene)
{
	picker.SetNear(scene.GetCamera().GetNear());
	picker.SetFar(scene.GetCamera().GetFar());

	MouseInput::Attach(this);
	KeyboardInput::Attach(this);
}

SceneEdit::~SceneEdit() {
	MouseInput::Detach(this);
	KeyboardInput::Attach(this);
}

bool SceneEdit::PickObject(const Ray& _ray) {
	if (!_ray.IsValid()) {
		assert(0);
		return false;
	}

	picker.SetRay(_ray);

	if (!picker.HitTest(scene)) {
		picker.Clear();
		scene.gizmos.Detach();

		return false;
	}

	if (scene.gizmos.IsAlreadyAttached()) {
		double distance = 0.0;
		picker.HitTest(scene.gizmos, distance);
		
		return true;
	}
	//Show Gizmo
	//..
	const GameObjects& selectedObjects = picker.GetSelectedObjects();
	if (!selectedObjects.empty()) {
		scene.gizmos.Attach(*selectedObjects.at(0));
	}

	return true;
}
Quaternion SceneEdit::CalcDragRotation(const GameObject& baseObject, const Point2i& prev, const Point2i& cur) {
	Gizmos::GizmoHandle::eHandle handleType = scene.gizmos.GetEditingHandle();
	
	Ray curRay = scene.GetRay(cur, scene.GetSceneSize());
	Ray prevRay = scene.GetRay(prev, scene.GetSceneSize());
	
	Volumef volume;
	baseObject.GetLocalBoundingBox(volume);

	Matrix<float, 4, 4> worldMatrix = baseObject.GetWorldMatrix();
	Vec4f vertices[8];
	vertices[0] = Transform(worldMatrix, Vec4f(volume.min.x, volume.max.y, volume.max.z, 1.0f));
	vertices[1] = Transform(worldMatrix, Vec4f(volume.max.x, volume.max.y, volume.max.z, 1.0f));
	vertices[2] = Transform(worldMatrix, Vec4f(volume.max.x, volume.max.y, volume.min.z, 1.0f));
	vertices[3] = Transform(worldMatrix, Vec4f(volume.min.x, volume.max.y, volume.min.z, 1.0f));
	vertices[4] = Transform(worldMatrix, Vec4f(volume.min.x, volume.min.y, volume.max.z, 1.0f));
	vertices[5] = Transform(worldMatrix, Vec4f(volume.max.x, volume.min.y, volume.max.z, 1.0f));
	vertices[6] = Transform(worldMatrix, Vec4f(volume.max.x, volume.min.y, volume.min.z, 1.0f));
	vertices[7] = Transform(worldMatrix, Vec4f(volume.min.x, volume.min.y, volume.min.z, 1.0f));

	Planef plane;
	switch (handleType) {
	case Gizmos::GizmoHandle::eHandle::TRANSLATE_X:
	case Gizmos::GizmoHandle::eHandle::TRANSLATE_Y:
		plane.Build(vertices[0], vertices[4], vertices[5]);
		break;
	case Gizmos::GizmoHandle::eHandle::TRANSLATE_Z:
		plane.Build(vertices[1], vertices[2], vertices[6]);
		break;
	}

	Vec3f centerPoint = volume.GetCenter();

	return Quaternion();
}

Vec3f SceneEdit::CalcDragOffsetInWorld(const GameObject& baseObject, const Point2i& prev, const Point2i& cur) {
	Gizmos::GizmoHandle::eHandle handleType = scene.gizmos.GetEditingHandle();
	if (handleType == Gizmos::GizmoHandle::eHandle::NONE)
		return Vec3f();

	Ray curRay = scene.GetRay(cur, scene.GetSceneSize());
	Ray prevRay = scene.GetRay(prev, scene.GetSceneSize());

	Volumef volume;
	baseObject.GetLocalBoundingBox(volume);

	Matrix<float, 4, 4> worldMatrix = baseObject.GetWorldMatrix();
	
	Vec4f vertices[8];
	vertices[0] = Transform(worldMatrix, Vec4f(volume.min.x, volume.max.y, volume.max.z, 1.0f));
	vertices[1] = Transform(worldMatrix, Vec4f(volume.max.x, volume.max.y, volume.max.z, 1.0f));
	vertices[2] = Transform(worldMatrix, Vec4f(volume.max.x, volume.max.y, volume.min.z, 1.0f));
	vertices[3] = Transform(worldMatrix, Vec4f(volume.min.x, volume.max.y, volume.min.z, 1.0f));
	vertices[4] = Transform(worldMatrix, Vec4f(volume.min.x, volume.min.y, volume.max.z, 1.0f));
	vertices[5] = Transform(worldMatrix, Vec4f(volume.max.x, volume.min.y, volume.max.z, 1.0f));
	vertices[6] = Transform(worldMatrix, Vec4f(volume.max.x, volume.min.y, volume.min.z, 1.0f));
	vertices[7] = Transform(worldMatrix, Vec4f(volume.min.x, volume.min.y, volume.min.z, 1.0f));

	Planef plane;
	switch (handleType) {
	case Gizmos::GizmoHandle::eHandle::TRANSLATE_X:
	case Gizmos::GizmoHandle::eHandle::TRANSLATE_Y:
		plane.Build(vertices[0], vertices[4], vertices[5]);
		break;
	case Gizmos::GizmoHandle::eHandle::TRANSLATE_Z:
		plane.Build(vertices[1], vertices[2], vertices[6]);
		break;
	}

	
	if (0 < plane.normal.DotProduct(curRay.GetDirection())) {
		plane.normal.Set(-plane.normal);
	}
	
	Vec3f distance = plane.GetIntersection(curRay.GetLine()) - plane.GetIntersection(prevRay.GetLine());
	//LogDebug(L"offset : %.8lf, %.8lf, %.8lf\n", distance.x, distance.y, distance.z);
	LogDebug(L"Plane - normal: %.8lf, %.8lf, %.8lf - distance: %.8lf\n", plane.normal.x, plane.normal.y, plane.normal.z, plane.distance);
	return ClampDragOffset(handleType, distance);
}

Vec3f SceneEdit::ClampDragOffset(Gizmos::GizmoHandle::eHandle handleType, const Vec3f& offset) {
	if (handleType == Gizmos::GizmoHandle::eHandle::NONE)
		return offset;

	Vec3f newOffset = offset;
	switch (handleType) {
	case Gizmos::GizmoHandle::eHandle::TRANSLATE_X:
		newOffset = Vec3f(offset.x, 0.0f, 0.0f);
		break;
	case Gizmos::GizmoHandle::eHandle::TRANSLATE_Y:
		newOffset = Vec3f(0.0f, offset.y, 0.0f);
		break;
	case Gizmos::GizmoHandle::eHandle::TRANSLATE_Z:
		newOffset = Vec3f(0.0f, 0.0f, offset.z);
		break;
	default:
		break;
	}
	return newOffset;
}
void SceneEdit::MoveSelectedObject(GameObjects& selection) {
	if (selection.empty())
		return;

	Vec3f offset = CalcDragOffsetInWorld(*selection.at(0), drag.GetBeginPoint(), drag.GetEndPoint());

	GameObject& selectedObj = *selection.at(0);
	objMemento.Restore(selectedObj);
	selectedObj.Move(offset);
}

void SceneEdit::RotateSelectedObject(GameObjects& selection) {
	if (selection.empty())
		return;

	Quaternion q = CalcDragRotation(*selection.at(0), drag.GetBeginPoint(), drag.GetEndPoint());

	GameObject& selectedObj = *selection.at(0);
	objMemento.Restore(selectedObj);
	//selectedObj.Rotate();
}

void SceneEdit::ProcessEvent(Event& e) {
	if (MouseInput::MouseEvent* pMouseEvent = dynamic_cast<MouseInput::MouseEvent*>(&e)) {
		if (pMouseEvent->mouseState == MouseInput::MouseEvent::MOUSE_STATE::MOUSE_STATE_COUNT) {
			return;
		}

		Point2i newMousePoint = pMouseEvent->newMousePoint;

		if (pMouseEvent->mouseState == MouseInput::MouseEvent::MOUSE_STATE::LBUTTON_DOWN) {
			drag.Begin(newMousePoint);

			const GameObjects& selectedObjects = picker.GetSelectedObjects();
			if(!selectedObjects.empty())
				objMemento.Set(*selectedObjects.at(0));
		}
		if (pMouseEvent->mouseState == MouseInput::MouseEvent::MOUSE_STATE::LBUTTON_UP) {
			drag.End();
		}

		if (pMouseEvent->mouseState == MouseInput::MouseEvent::MOUSE_STATE::MOUSE_MOVE) {
			drag.Track(newMousePoint);

			if (drag.IsTracked()) {
				DoDrag(dragMode);
			}else {
				DoFocus(newMousePoint);
			}
		}
	}

	if (KeyboardInput::KeyboardEvent* pKeyboardEvent = dynamic_cast<KeyboardInput::KeyboardEvent*>(&e)) {
		if (pKeyboardEvent->keyState == KeyboardInput::KeyboardEvent::KEY_STATE::KEY_STATE_UP) {
			const KeyInfo& keyInfo = pKeyboardEvent->GetInfo();
			if (keyInfo.key == KEY_R) {
				dragMode = eDragMode::DRAG_MODE_ROTATING;
				scene.SetTransformMode(Gizmos::eTransformMode::ROTATE);
			}
			if (keyInfo.key == KEY_T) {
				dragMode = eDragMode::DRAG_MODE_MOVING;
				scene.SetTransformMode(Gizmos::eTransformMode::TRANSLATE);
			}
		}
	}
}

void SceneEdit::DoFocus(const Point2i& pt) {
	Ray curRay = scene.GetRay(pt, scene.GetSceneSize());
	double distance = DBL_MAX;

	picker.SetRay(curRay);
	if (scene.gizmos.IsAlreadyAttached()) {
		if (picker.HitTest(scene.gizmos, distance)) {
		}
	}
}
void SceneEdit::DoDrag(eDragMode dragMode) {
	if (dragMode == eDragMode::DRAG_MODE_NONE)
		return;

	GameObjects selectedObjects = picker.GetSelectedObjects();
	if (dragMode == eDragMode::DRAG_MODE_MOVING) {
		MoveSelectedObject(selectedObjects);
	}else if (dragMode == eDragMode::DRAG_MODE_ROTATING) {

	}
}