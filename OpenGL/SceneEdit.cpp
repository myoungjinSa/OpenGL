#include "SceneEdit.h"
#include "RayCast.h"
#include "Input.h"
#include "Logger.h"

void SceneEdit::ObjectMemento::Set(const GameObject& gameObject) {
	position = gameObject.GetPosition();
	scale = gameObject.GetScale();
	orientation = gameObject.transform->GetOrientation();
	look = gameObject.GetLook();
	up = gameObject.GetUp();
	right = gameObject.GetRight();
}
void SceneEdit::ObjectMemento::Restore(GameObject& gameObject) {
	gameObject.SetPosition(position);
	gameObject.SetScale(scale);
	gameObject.SetOrientation(orientation);
	gameObject.SetLookRightUp(look, right, up);
}


SceneEdit::SceneEdit(class Scene& _scene)
	: scene(_scene)
{
	picker.SetNear(scene.GetCamera().GetNear());
	picker.SetFar(scene.GetCamera().GetFar());

	bWorldCoordMode = false;

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
		LogDebug(L"Hit Distance = %.8lf\n", distance);

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

Vec3f SceneEdit::CalcMoveOffset(const GameObject& baseObject, const Point2i& prev, const Point2i& cur) {
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
	case Gizmos::GizmoHandle::eHandle::TRANSLATE_XY:
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
	return ClampDragOffset(handleType, distance);
}

Vec3f SceneEdit::CalcScaleFactor(const GameObject& baseObject, const Point2i& prev, const Point2i& cur) {
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
	case Gizmos::GizmoHandle::eHandle::SCALE_X:
	case Gizmos::GizmoHandle::eHandle::SCALE_Y:
		plane.Build(vertices[0], vertices[4], vertices[5]);
		break;
	case Gizmos::GizmoHandle::eHandle::SCALE_Z:
		plane.Build(vertices[1], vertices[2], vertices[6]);
		break;
	}

	Vec3f distance = plane.GetIntersection(curRay.GetLine()) - plane.GetIntersection(prevRay.GetLine());
	Vec3f scaleFactor = ClampDragOffset(handleType, distance);
	return scaleFactor;
}

Matrix<float, 3, 3> SceneEdit::CalcRotationMatrix(const GameObject& baseObject, const Vec3f& prevPickedPoint, const Point2i& cur) {
	Gizmos::GizmoHandle::eHandle handleType = scene.gizmos.GetEditingHandle();
	if (handleType == Gizmos::GizmoHandle::eHandle::NONE)
		return Matrix<float, 4, 4>::Identity();
	
	Vec3f targetPosition = baseObject.GetPosition();
	if (handleType == Gizmos::GizmoHandle::eHandle::ROTATE_XY) {
		Planef viewPlane;
		viewPlane.normal = scene.GetCamera().GetPosition() - targetPosition;
		viewPlane.normal.Normalize();
		viewPlane.distance = -viewPlane.normal.DotProduct(prevPickedPoint);

		Ray curRay = scene.GetRay(cur, scene.GetSceneSize());
		Vec3f curPickedPoint = viewPlane.GetIntersection(curRay.GetLine());

		Vec3f toPrevPickedDir = prevPickedPoint - targetPosition;
		Vec3f toCurPickedDir = curPickedPoint - targetPosition;

		Vec3f axis = toPrevPickedDir.Cross(toCurPickedDir);

		Vec3f direction = toCurPickedDir - toPrevPickedDir;
		Vec3f curLength = curPickedPoint - prevPickedPoint;

		float distance = direction.DotProduct(curLength);
		float radius = scene.gizmos.CalcGizmoSize(scene.GetCamera()).x;
		float radian = distance / (radius * MathUtils::PI) * 2.0f;

		Matrix<float, 3, 3> rotationMatrix = Matrix<float, 3, 3>::Identity();
		rotationMatrix = MathUtils::CalculateRotationMatrix(axis, radian);
		return rotationMatrix;
	}
	else {
		Vec3f axis;
		switch (handleType) {
		case Gizmos::GizmoHandle::eHandle::ROTATE_X:
		{	//Local Coordinate
			if (bWorldCoordMode) {
				axis.Set(1.0f, 0.0f, 0.0f);
			}else {
				Vec3f right = baseObject.GetRight();
				axis.Set(right);
			}
			break;
		}
		case Gizmos::GizmoHandle::eHandle::ROTATE_Y:
		{
			if (bWorldCoordMode) {
				axis.Set(0.0f, 1.0f, 0.0f);
			}else {
				Vec3f up = baseObject.GetUp();
				axis.Set(up);
			}
			break;
		}
		case Gizmos::GizmoHandle::eHandle::ROTATE_Z:
		{	
			if (bWorldCoordMode) {
				axis.Set(0.0f, 0.0f, 1.0f);
			}else {
				Vec3f look = baseObject.GetLook();
				axis.Set(look);
			}
			break;
		}
		}

		Vec3f toPrevPickedDir = prevPickedPoint - targetPosition;
		Vec3f direction = axis.Cross(toPrevPickedDir);
		direction.Normalize();

		Planef viewPlane;
		viewPlane.normal = scene.GetCamera().GetPosition() - targetPosition;
		viewPlane.normal.Normalize();
		viewPlane.distance = -viewPlane.normal.DotProduct(prevPickedPoint);
		
		Ray curRay = scene.GetRay(cur, scene.GetSceneSize());
		Vec3f curPickedPoint = viewPlane.GetIntersection(curRay.GetLine());
		
		Vec3f curLength = curPickedPoint - prevPickedPoint;
		float distance = direction.DotProduct(curLength);
		float radius = scene.gizmos.CalcGizmoSize(scene.GetCamera()).x;
		float radian = distance / (radius * MathUtils::PI) * 2.0f;

		Matrix<float, 3, 3> rotationMatrix = Matrix<float, 3, 3>::Identity();
		rotationMatrix = MathUtils::CalculateRotationMatrix(axis, radian);
		return rotationMatrix;
	}
	assert(0);
	return Matrix<float, 4, 4>::Identity();
}

Quaternion SceneEdit::CalcRotationQuaternion(const GameObject& baseObject, const Vec3f& prevPickedPoint, const Point2i& cur) {
	Gizmos::GizmoHandle::eHandle handleType = scene.gizmos.GetEditingHandle();
	if (handleType == Gizmos::GizmoHandle::eHandle::NONE)
		return Quaternion();

	Vec3f targetPosition = baseObject.GetPosition();
	Planef viewPlane;
	viewPlane.normal = scene.GetCamera().GetPosition() - targetPosition;
	viewPlane.normal.Normalize();
	viewPlane.distance = -viewPlane.normal.DotProduct(prevPickedPoint);

	Ray curRay = scene.GetRay(cur, scene.GetSceneSize());
	Vec3f curPickedPoint = viewPlane.GetIntersection(curRay.GetLine());

	Vec3f toPrevPickedDir = prevPickedPoint - targetPosition;
	Vec3f toCurPickedDir = curPickedPoint - targetPosition;
	
	Vec3f axis = toPrevPickedDir.Cross(toCurPickedDir);

	Vec3f direction = toCurPickedDir - toPrevPickedDir;
	Vec3f curLength = curPickedPoint - prevPickedPoint;
	
	float distance = direction.DotProduct(curLength);
	float radius = scene.gizmos.CalcGizmoSize(scene.GetCamera()).x;
	float radian = distance / (radius * MathUtils::PI) * 2.0f;


	Quaternion q = MathUtils::GetRotatedQuaternion(axis, radian);
	q.Normalize();
	return q;
}

Vec3f SceneEdit::ClampDragOffset(Gizmos::GizmoHandle::eHandle handleType, const Vec3f& offset) {
	if (handleType == Gizmos::GizmoHandle::eHandle::NONE)
		return offset;

	Vec3f newOffset = offset;
	switch (handleType) {
	case Gizmos::GizmoHandle::eHandle::TRANSLATE_X:
	case Gizmos::GizmoHandle::eHandle::SCALE_X:
		newOffset = Vec3f(offset.x, 0.0f, 0.0f);
		break;
	case Gizmos::GizmoHandle::eHandle::TRANSLATE_Y:
	case Gizmos::GizmoHandle::eHandle::SCALE_Y:
		newOffset = Vec3f(0.0f, offset.y, 0.0f);
		break;
	case Gizmos::GizmoHandle::eHandle::TRANSLATE_Z:
	case Gizmos::GizmoHandle::eHandle::SCALE_Z:
		newOffset = Vec3f(0.0f, 0.0f, offset.z);
		break;
	case Gizmos::GizmoHandle::eHandle::TRANSLATE_XY:
		newOffset = Vec3f(offset.x, offset.y, 0.0f);
		break;
	default:
		break;
	}
	return newOffset;
}
void SceneEdit::MoveSelectedObjects(GameObjects& selectedObjects) {
	if (selectedObjects.empty())
		return;

	GameObject& selectedObj = *selectedObjects.at(0);

	Vec3f offset = CalcMoveOffset(selectedObj, drag.GetBeginPoint(), drag.GetEndPoint());
	objMemento.Restore(selectedObj);
	selectedObj.Move(offset);
}

void SceneEdit::RotateSelectedObjects(GameObjects& selectedObjects) {
	if (selectedObjects.empty())
		return;

	GameObject& selectedObj = *selectedObjects.at(0);
	objMemento.Restore(selectedObj);

	Vec3f PrevPoint = Normalize(picker.GetRay().GetDirection()) + picker.GetRay().GetPosition();

	Matrix<float, 3, 3> rotationMatrix = CalcRotationMatrix(selectedObj, PrevPoint, drag.GetEndPoint());
	selectedObj.Rotate(rotationMatrix);
}

void SceneEdit::ScaleSelectedObjects(GameObjects& selectedObjects) {
	if (selectedObjects.empty())
		return;

	GameObject& selectedObj = *selectedObjects.at(0);
	objMemento.Restore(selectedObj);

	Vec3f scaleFactor = CalcScaleFactor(selectedObj, drag.GetBeginPoint(), drag.GetEndPoint());
	Vec3f orgScale = selectedObj.GetScale();
	
	const float minScale = 0.25f;
	Vec3f newScale = orgScale + scaleFactor;
	newScale = Vec3f(MAX(newScale.x, minScale), MAX(newScale.y, minScale), MAX(newScale.z, minScale));
	selectedObj.SetScale(newScale);
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
			}else if (keyInfo.key == KEY_T) {
				scene.SetTransformMode(Gizmos::eTransformMode::TRANSLATE);
			}else if (keyInfo.key == KEY_E) {
				scene.SetTransformMode(Gizmos::eTransformMode::SCALE);
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
		MoveSelectedObjects(selectedObjects);
	}else if (scene.gizmos.GetTransformMode() == Gizmos::eTransformMode::ROTATE) {
		RotateSelectedObjects(selectedObjects);
	}else if (scene.gizmos.GetTransformMode() == Gizmos::eTransformMode::SCALE) {
		ScaleSelectedObjects(selectedObjects);
	}
}