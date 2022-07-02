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

Quaternion SceneEdit::CalcRotationDegree(const GameObject& baseObject, const Vec3f& prevPickedPoint, const Point2i& cur) {
	Gizmos::GizmoHandle::eHandle handleType = scene.gizmos.GetEditingHandle();
	if (handleType == Gizmos::GizmoHandle::eHandle::NONE)
		return Quaternion();

	Ray curRay = scene.GetRay(cur, scene.GetSceneSize());
	
	Matrix<float, 3, 3> orgRotationMatrix = Matrix<float, 3, 3>::Identity();
	orgRotationMatrix = baseObject.transform->GetRotationMatrix();

	Vec3f targetPosition = baseObject.GetPosition();

	Planef viewPlane;
	viewPlane.normal = scene.GetCamera().GetPosition() - targetPosition;
	viewPlane.normal.Normalize();
	viewPlane.distance = -viewPlane.normal.DotProduct(prevPickedPoint);

	Vec3f curPickedPoint = viewPlane.GetIntersection(curRay.GetLine());

	Vec3f pickedDir = prevPickedPoint - targetPosition;
	Vec3f curDir = curPickedPoint - targetPosition;
	Vec3f axis = curDir.Cross(pickedDir);

	switch (handleType) {
	case Gizmos::GizmoHandle::eHandle::ROTATE_X:
		axis = Vec3f(axis.x, 0.0f, 0.0f);
		break;
	case Gizmos::GizmoHandle::eHandle::ROTATE_Y:
		axis = Vec3f(0.0f, axis.y, 0.0f);
		break;
	case Gizmos::GizmoHandle::eHandle::ROTATE_Z:
		axis = Vec3f(0.0f, 0.0f, axis.z);
		break;
	}

	Vec3f direction = curDir - pickedDir;
	direction.Normalize();
	
	Vec3f curLength = curPickedPoint - prevPickedPoint;
	float distance = direction.DotProduct(curLength);
	Vec3f radius = scene.gizmos.CalcGizmoSize(scene.GetCamera());	
	float radian = distance / (radius.x * MathUtils::PI) * 2.0f;
	float degree = MathUtils::RadiansToDegrees(radian);

	Quaternion q = MathUtils::GetRotatedQuaternion(axis, degree);
	return q;
}
Vec4d SceneEdit::CalcDragRotation(const GameObject& baseObject, const Point2i& prev, const Point2i& cur, const Vec3f& axis, const Vec3f& startPosition, const Vec4d& startOrientation) {
	Gizmos::GizmoHandle::eHandle handleType = scene.gizmos.GetEditingHandle();

	Ray curRay = scene.GetRay(cur, scene.GetSceneSize());
	Ray prevRay = scene.GetRay(prev, scene.GetSceneSize());

	Vec3f scale = Vec3f(1.0f, 1.0f, 1.0f);
	Vec3f theAxis = startPosition + (Vec3f)Quaternion::Rotate(startOrientation, axis * scale);

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
	case Gizmos::GizmoHandle::eHandle::ROTATE_X:
	case Gizmos::GizmoHandle::eHandle::ROTATE_Y:
		plane.Build(vertices[0], vertices[4], vertices[5]);
		break;
	case Gizmos::GizmoHandle::eHandle::ROTATE_Z:
		plane.Build(vertices[1], vertices[2], vertices[6]);
		break;
	}
	Vec3f intersectionPoint = plane.GetIntersection(curRay.GetLine());
	Vec3f clickOffset = baseObject.transform->TransformPoint(intersectionPoint);

	Vec3f originalPosition = startPosition;
	Vec4d orientation = baseObject.transform->GetOrientation();

	Vec3f centerOfRotation = originalPosition + theAxis * DotProduct(theAxis, clickOffset - originalPosition);
	Vec3f arm1 = Normalize(clickOffset - centerOfRotation);
	Vec3f arm2 = Normalize(intersectionPoint - centerOfRotation);

	float d = DotProduct(arm1, arm2);
	if (0.999f < d) {
		orientation = startOrientation;
		return orientation;
	}

	float angle = std::acos(d);
	if (angle < 0.001f) {
		orientation = startOrientation;
		return orientation;
	}

	auto newAxis = Normalize(Cross(arm1, arm2));
	Quaternion rotated = MathUtils::GetRotatedQuaternion(newAxis, angle);
	Quaternion newOrientation = newOrientation.Multiply(rotated.GetVector(), startOrientation);
	return newOrientation.GetVector();

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

	GameObject& selectedObj = *selection.at(0);
	objMemento.Restore(selectedObj);

	Vec3f PrevPoint = Normalize(picker.GetRay().GetDirection()) + picker.GetRay().GetPosition();
	Quaternion q = CalcRotationDegree(*selection.at(0), PrevPoint, drag.GetEndPoint());
	selectedObj.Rotate(q);
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
				DoDrag();
			}else {
				DoFocus(newMousePoint);
			}
		}
	}

	if (KeyboardInput::KeyboardEvent* pKeyboardEvent = dynamic_cast<KeyboardInput::KeyboardEvent*>(&e)) {
		if (pKeyboardEvent->keyState == KeyboardInput::KeyboardEvent::KEY_STATE::KEY_STATE_UP) {
			const KeyInfo& keyInfo = pKeyboardEvent->GetInfo();
			if (keyInfo.key == KEY_R) {
				scene.SetTransformMode(Gizmos::eTransformMode::ROTATE);
			}
			if (keyInfo.key == KEY_T) {
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
void SceneEdit::DoDrag() {
	GameObjects selectedObjects = picker.GetSelectedObjects();
	if (scene.gizmos.GetTransformMode() == Gizmos::eTransformMode::TRANSLATE) {
		MoveSelectedObject(selectedObjects);
	}else if (scene.gizmos.GetTransformMode() == Gizmos::eTransformMode::ROTATE) {
		RotateSelectedObject(selectedObjects);
	}
}