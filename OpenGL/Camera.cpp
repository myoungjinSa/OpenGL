#include "Camera.h"
#include "Input.h"
#include "Logger.h"
#include "Quaternion.h"

Camera::Camera()
	:GameObject()
	,width(0)
	,height(0)
	,viewMatrix()
	,oldMousePoint()
	,xAngle(0.0f)
	,yAngle(0.0f)
{
	
}
Camera::~Camera() {

}

bool Camera::Initialize(Renderer& renderer) {
	MouseInput::Attach(this);
	KeyboardInput::Attach(this);
	return true;
}

void Camera::Render(Renderer& renderer) {

}

void Camera::Shutdown(Renderer& renderer) {
	MouseInput::Detach(this);
	KeyboardInput::Detach(this);
}

void Camera::Update(float deltaTime) {
	float yaw = 0.0f, pitch = 0.0f, roll = 0.0f;

	const float rotationSpeed = 0.0174532925f;
	
	pitch = xAngle * rotationSpeed;
	yaw = yAngle * rotationSpeed;
	roll = 0.0f;

	Vec3f up(0.0f, 1.0f, 0.0f), lookAt(0.0f, 0.0f, 1.0f);
	
	Matrix<float, 3, 3> rotationMatrix = Matrix<float, 3, 3>::Identity();
	MatrixRotationYawPitchRoll(rotationMatrix, yaw, pitch, roll);

	lookAt = Transform(rotationMatrix, lookAt);
	up = Transform(rotationMatrix, up);

	//LogDebug(L"lookAt - (%.8lf, %.8lf, %.8lf) up - (%.8lf, %.8lf, %.8lf)\n", lookAt.x, lookAt.y, lookAt.z, up.x, up.y, up.z);

	float movingSpeed = transform.get()->GetMovingSpeed();
	if (KeyboardInput::IsKeyDown(KEY_D)) {
		Move(GetRight() * 1.0f, movingSpeed, deltaTime);
	}
	else if (KeyboardInput::IsKeyDown(KEY_A)) {
		Move(GetRight() * -1.0f, movingSpeed, deltaTime);
	}
	else if (KeyboardInput::IsKeyDown(KEY_W)) {
		Move(GetLook(), movingSpeed, deltaTime);
	}
	else if (KeyboardInput::IsKeyDown(KEY_S)) {
		Move(GetLook() * -1.0f, movingSpeed, deltaTime);
	}

	BuildViewMatrix(GetPosition() + lookAt, up);
}

void Camera::MatrixRotationYawPitchRoll(Matrix<float, 3, 3>& matrix, float yaw, float pitch, float roll) {
	Quaternion q;
	q.GetEuler(Vec3d(pitch, yaw, roll));
	q.Normalize();

	Matrix<double, 3, 3> m = q.GetRotationMatrix();

	matrix[0] = m[0];
	matrix[1] = m[3];
	matrix[2] = m[6];
	
	matrix[3] = m[1];
	matrix[4] = m[4];
	matrix[5] = m[7];
	
	matrix[6] = m[2];
	matrix[7] = m[5];
	matrix[8] = m[8];
}


void Camera::SetViewport(const Rect2f& _viewport) {
	if (_viewport.Empty())
		return;


	SetWidth(_viewport.width);
	SetHeight(_viewport.height);
	viewport = _viewport;
}

void Camera::BuildViewMatrix(Vec3f lookAt, Vec3f up) {
	Vec3f xAxis, yAxis, zAxis;
	Vec3f position = transform.get()->GetPosition();
	zAxis = Normalize(lookAt - position);

	xAxis = Cross(up, zAxis);
	xAxis = Normalize(xAxis);

	yAxis = Cross(zAxis, xAxis);

	float xPos = DotProduct(xAxis, position) * -1.0f;
	float yPos = DotProduct(yAxis, position) * -1.0f;
	float zPos = DotProduct(zAxis, position) * -1.0f;
	
	viewMatrix.value[0]  = xAxis.x;
	viewMatrix.value[1]  = yAxis.x;
	viewMatrix.value[2]  = zAxis.x;
	viewMatrix.value[3]  = 0.0f;
						 
	viewMatrix.value[4]  = xAxis.y;
	viewMatrix.value[5]  = yAxis.y;
	viewMatrix.value[6]  = zAxis.y;
	viewMatrix.value[7]  = 0.0f;

	viewMatrix.value[8]  = xAxis.z;
	viewMatrix.value[9]  = yAxis.z;
	viewMatrix.value[10] = zAxis.z;
	viewMatrix.value[11] = 0.0f;

	viewMatrix.value[12] = xPos;
	viewMatrix.value[13] = yPos;
	viewMatrix.value[14] = zPos;
	viewMatrix.value[15] = 1.0f;
}



void Camera::GetViewMatrix(Matrix<float, 4, 4>& Matrix) const {
	for (int iVal = 0; iVal < Matrix.rows * Matrix.cols; iVal++) {
		Matrix[iVal] = viewMatrix[iVal];
	}
}

void Camera::BuildPerspectiveFovLHMatrix(Matrix<float, 4, 4>& matrix, const Rect2f& viewport, float screenNear, float screenDepth) {
	float fov = GetFov();
	matrix[0] = (1.0f / (GetAspectRatio() * tan(fov * 0.5f)));
	matrix[1] = 0.0f;
	matrix[2] = 0.0f;
	matrix[3] = 0.0f;

	matrix[4] = 0.0f;
	matrix[5] = 1.0f / tan(fov * 0.5f);
	matrix[6] = 0.0f;
	matrix[7] = 0.0f;

	matrix[8] = 0.0f;
	matrix[9] = 0.0f;
	matrix[10] = screenDepth / (screenDepth - screenNear);
	matrix[11] = 1.0f;

	matrix[12] = 0.0f;
	matrix[13] = 0.0f;
	matrix[14] = (-screenNear * screenDepth) / (screenDepth - screenNear);
	matrix[15] = 0.0f;
}


void Camera::ProcessEvent(Event& e) {
	if (MouseInput::MouseEvent* pMouseEvent = dynamic_cast<MouseInput::MouseEvent*>(&e)) {
		if (pMouseEvent->mouseState == MouseInput::MouseEvent::MOUSE_STATE::MOUSE_STATE_COUNT)
			return;
		

		Point2i newMousePoint = pMouseEvent->mousePoint;
		Vec2f mouseDelta(newMousePoint.x - oldMousePoint.x, newMousePoint.y - oldMousePoint.y);
		
		if (pMouseEvent->mouseState == MouseInput::MouseEvent::MOUSE_STATE::MOUSE_DRAG) {
			if (sqrtf(newMousePoint.x - oldMousePoint.x + newMousePoint.y - oldMousePoint.y) > 50.0f)
			{
				oldMousePoint = newMousePoint;
				return;
			}
			const float rotationSpeed = 0.5f;

			xAngle += (newMousePoint.y - oldMousePoint.y) / 3.6f;
			yAngle += (newMousePoint.x - oldMousePoint.x) / 3.6f;
		}

		oldMousePoint = newMousePoint;
	}

	if (dynamic_cast<KeyboardInput::KeyboardEvent*>(&e)) {
		//LogDebug(L"Keyboard Event Occured.\n");


	}

}
