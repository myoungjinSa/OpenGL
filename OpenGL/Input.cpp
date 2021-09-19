#include "Input.h"
#include "System.h"
#include "Logger.h"
#include <fstream>
#include <sstream>

std::map<unsigned int, std::pair<const String, bool>> KeyboardInput::keyCodes;
std::vector<Observer*> KeyboardInput::observers;
bool KeyboardInput::Initialize() {
	return LoadKeyCodes("keyCodes.txt");
}

void KeyboardInput::KeyDown(unsigned int input)
{
	keyCodes[input].second = true;

	std::vector<unsigned int> keys;
	keys.push_back(input);
	KeyboardEvent e(KeyboardEvent::KEY_STATE::KEY_STATE_DOWN, keys);
	Notify(e);
}


void KeyboardInput::KeyUp(unsigned int input)
{
	keyCodes[input].second = false;

	std::vector<unsigned int> keys;
	keys.push_back(input);
	KeyboardEvent e(KeyboardEvent::KEY_STATE::KEY_STATE_UP, keys);
	Notify(e);
}


bool KeyboardInput::IsKeyDown(unsigned int input)
{
	if (!keyCodes[input].second)
		return false;

	return true;
}

bool KeyboardInput::LoadKeyCodes(String&& keyFileName) {
	std::ifstream keyFile(keyFileName.c_str(), std::ios::in);
	if (!keyFile.is_open()) {
		assert(0);
		return false;
	}

	std::map<unsigned int, std::pair<const String, bool>>::iterator iter = keyCodes.begin();
	while (!keyFile.eof()) {
		char _keyName[128];
		keyFile.getline(_keyName, 128);

		String keyName = _keyName;
		std::vector<String> keyComponents = Split(keyName, ',');
		keyCodes.insert(iter, std::make_pair((unsigned int)StringToInt(keyComponents.at(1)), std::make_pair(keyComponents.at(0), false)));
	}

	return true;
}


void KeyboardInput::Attach(Observer* pObserver) {
	observers.push_back(pObserver);;
}

bool KeyboardInput::Detach(Observer* pObserver) {
	observers.erase(std::remove_if(observers.begin(), observers.end(), [pObserver](Observer* pOb) {
if (pOb == pObserver)
return true;
else
return false;
		}), observers.end());
		return true;
}
void KeyboardInput::Notify(KeyboardInput::KeyboardEvent& e) {
	for (auto pObserver : observers) {
		pObserver->ProcessEvent(e);
	}
}

KeyboardInput::KeyboardEvent::KeyboardEvent(KEY_STATE _keyState, const std::vector<unsigned int>& _keys) {
	for (unsigned int key : keys) {
		keys.push_back(key);
	}

	keyState = _keyState;
}
void KeyboardInput::KeyboardEvent::GetInfo() {

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//MouseInput


float MouseInput::xAngle = 0.0f;
float MouseInput::yAngle = 0.0f;
bool MouseInput::dragging = false;
bool MouseInput::leftButtonDown = false;
bool MouseInput::rightButtonDown = false;
Point2i MouseInput::mousePoint;
std::vector<Observer*> MouseInput::observers;

bool MouseInput::Initialize() {

	return true;
}
void MouseInput::ProcessLButtonDown(int x, int y) {
	leftButtonDown = true;
	dragging = !dragging;
	if (dragging) {
		mousePoint.x = x;
		mousePoint.y = y;

		MouseEvent e(MouseEvent::MOUSE_STATE::LBUTTON_DOWN, Point2i(mousePoint.x, mousePoint.y));
		Notify(e);
	}
}


void MouseInput::ProcessLButtonUp(int x, int y) {
	leftButtonDown = false;
	if (dragging) {
		dragging = false;
		MouseEvent e(MouseEvent::MOUSE_STATE::LBUTTON_UP, Point2i(x, y));
		Notify(e);
	}
}

void MouseInput::ProcessRButtonDown(int x, int y) {
	rightButtonDown = true;
	dragging = !dragging;
	if (dragging) {
		mousePoint.x = x;
		mousePoint.y = y;

		MouseEvent e(MouseEvent::MOUSE_STATE::RBUTTON_DOWN, Point2i(mousePoint.x, mousePoint.y));
		Notify(e);
	}
}

void MouseInput::ProcessRButtonUp(int x, int y) {
	rightButtonDown = false;
	if (dragging) {
		dragging = false;
		MouseEvent e(MouseEvent::MOUSE_STATE::RBUTTON_UP, Point2i(x, y));
		Notify(e);
	}
}


void MouseInput::ProcessMouseMove(int x, int y) {
	if (mousePoint.x == x && mousePoint.y == y)
		return;

	if (dragging) {
		int dragX = x;
		int dragY = y;

		mousePoint.x = dragX;
		mousePoint.y = dragY;
		//dragY = y;

		if (rightButtonDown) {
			MouseEvent e(MouseEvent::MOUSE_STATE::RIGHT_BUTTON_DRAG, Point2i(mousePoint.x, mousePoint.y));
			Notify(e);
		}
		if (leftButtonDown) {
			MouseEvent e(MouseEvent::MOUSE_STATE::LEFT_BUTTON_DRAG, Point2i(mousePoint.x, mousePoint.y));
			Notify(e);
		}
	}
}

float MouseInput::GetXAngle() {
	return xAngle;
}

float MouseInput::GetYAngle() {
	return yAngle;
}


void MouseInput::Attach(Observer* pObserver) {
	observers.push_back(pObserver);
}

bool MouseInput::Detach(Observer* pObserver) {
	observers.erase(std::remove_if(observers.begin(), observers.end(), [pObserver](Observer* pOb) {
		if (pOb == pObserver)
			return true;
		else
			return false;
		}), observers.end());
	return true;
}

void MouseInput::Notify(MouseInput::MouseEvent& e) {
	for (auto pObserver : observers) {
		pObserver->ProcessEvent(e);
	}
}

MouseInput::MouseEvent::MouseEvent(MOUSE_STATE _mouseState, const Point2i& _mousePoint) {
	mousePoint = _mousePoint;
	mouseState = _mouseState;
}
void MouseInput::MouseEvent::GetInfo() {
	
}
