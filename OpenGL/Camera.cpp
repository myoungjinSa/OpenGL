#include "Camera.h"
#include "Input.h"
#include "Logger.h"
#include "Quaternion.h"

Viewport::Viewport() 
	: rect()
{

}

Viewport::Viewport(const Rect2f& rect) 
{
	SetViewport(rect);
}


void Viewport::SetViewport(const Rect2f& _rect) {
	if (_rect.Empty())
		return;

	rect = _rect;

	if (rect.GetWidth() <= 0.0f) {
		rect.right = rect.left + 1.0f;
	}
	if (rect.GetHeight() <= 0.0f) {
		rect.bottom = rect.top + 1.0f;
	}

	halfWidth = rect.GetWidth() / 2.0f;
	halfHeight = rect.GetHeight() / 2.0f;

	centerX = (rect.left + rect.right) / 2.0f;
	centerY = (rect.top + rect.bottom) / 2.0f;
}

Camera::Camera()
	:GameObject()
	,viewMatrix()
	,Near(0.1f)
	,Far(1000.0f)
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



void Camera::Shutdown(Renderer& renderer) {
	MouseInput::Detach(this);
	MouseInput::Detach(this);
}

void Camera::Update(float deltaTime) {
	const float rotationSpeed = 0.0174532925f;
	
	float pitch = 0.0f, yaw = 0.0f, roll = 0.0f;
	pitch = xAngle * rotationSpeed;
	yaw = yAngle * rotationSpeed;

	Matrix<float, 3, 3> rotationMatrix = Matrix<float, 3, 3>::Identity();
	MatrixRotationYawPitchRoll(rotationMatrix, yaw, pitch, roll);
	BuildViewMatrix(rotationMatrix);
	
	float movingSpeed = transform.get()->GetMovingSpeed();
	if (KeyboardInput::IsKeyDown(KEY_D)) {
		Move(GetRight() * 1.0f, movingSpeed, deltaTime);
	}
	if (KeyboardInput::IsKeyDown(KEY_A)) {
		Move(GetRight() * -1.0f, movingSpeed, deltaTime);
	}
	if (KeyboardInput::IsKeyDown(KEY_W)) {
		Move(GetLook(), movingSpeed, deltaTime);
	}
	if (KeyboardInput::IsKeyDown(KEY_S)) {
		Move(GetLook() * -1.0f, movingSpeed, deltaTime);
	}
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
	viewport.SetViewport(_viewport);
}

void Camera::BuildViewMatrix(const Matrix<float, 3, 3>& rotationMatrix) {
	Vec3f up(0.0f, 1.0f, 0.0f), lookAt(0.0f, 0.0f, 1.0f), right(1.0f, 0.0f, 0.0f);

	lookAt = Transform(rotationMatrix, lookAt);
	up = Transform(rotationMatrix, up);
	right = Normalize(Cross(up, lookAt));

	Vec3f xAxis, yAxis, zAxis;
	Vec3f position = transform.get()->GetPosition();
	zAxis = Normalize(lookAt);

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

Vec3f Camera::GetRight()const{
	return Vec3f(viewMatrix.Col(0));
}

Vec3f Camera::GetUp() const {
	return Vec3f(viewMatrix.Col(1));
}

Vec3f Camera::GetLook() const {
	return Vec3f(viewMatrix.Col(2));
}

void Camera::BuildPerspectiveFovLHMatrix(Matrix<float, 4, 4>& matrix) {
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
	matrix[10] = Far / (Far - Near);
	matrix[11] = 1.0f;

	matrix[12] = 0.0f;
	matrix[13] = 0.0f;
	matrix[14] = (-Near * Far) / (Far - Near);
	matrix[15] = 0.0f;
}

void Camera::ProcessEvent(Event& e) {
	if (MouseInput::MouseEvent* pMouseEvent = dynamic_cast<MouseInput::MouseEvent*>(&e)) {
		if (pMouseEvent->mouseState == MouseInput::MouseEvent::MOUSE_STATE::MOUSE_STATE_COUNT) {
			return;
		}

		Point2i newMousePoint = pMouseEvent->newMousePoint;
		Point2i oldMousePoint = pMouseEvent->oldMousePoint;
		if (pMouseEvent->mouseState == MouseInput::MouseEvent::MOUSE_STATE::MOUSE_MOVE) {
			if (MouseInput::rightButtonDown) {
				xAngle += (newMousePoint.y - oldMousePoint.y) / 3.0f;
				yAngle += (newMousePoint.x - oldMousePoint.x) / 3.0f;
			}
		}
	}
}