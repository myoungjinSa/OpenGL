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
#if defined(_WIN64) || defined(_WIN32)
	return (GetAsyncKeyState(input) & 0x8000) ? true : false;
#else
	if (!keyCodes[input].second)
		return false;

	return true;
#endif
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

KeyInfo::KeyInfo() 
	: bToggleCtrl(false), bToggleMenu(false), bToggleShift(false), key(0)
{

}

KeyboardInput::KeyboardEvent::KeyboardEvent(KEY_STATE _keyState, const std::vector<unsigned int>& _keys) {
	for (unsigned int key : _keys) {
		keys.push_back(key);
	}

	keyState = _keyState;
}
const KeyInfo& KeyboardInput::KeyboardEvent::GetInfo() {
	KeyInfo keyInfo;
	keyInfo.key = keys.back();
	return KeyInfo(keyInfo);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//MouseInput


bool MouseInput::leftButtonDown = false;
bool MouseInput::rightButtonDown = false;
Point2i MouseInput::mousePoint;
Point2i MouseInput::oldMousePoint;
Drag MouseInput::drag;
std::vector<Observer*> MouseInput::observers;

bool MouseInput::Initialize() {

	return true;
}
void MouseInput::ProcessLButtonDown(int x, int y) {
	leftButtonDown = true;

	mousePoint.x = x;
	mousePoint.y = y;

	drag.Begin(mousePoint);

	MouseEvent e(MouseEvent::MOUSE_STATE::LBUTTON_DOWN, mousePoint, oldMousePoint);
	Notify(e);
}


void MouseInput::ProcessLButtonUp(int x, int y) {
	leftButtonDown = false;

	drag.End();

	MouseEvent e(MouseEvent::MOUSE_STATE::LBUTTON_UP, Point2i(x, y), oldMousePoint);
	Notify(e);
}

void MouseInput::ProcessRButtonDown(int x, int y) {
	rightButtonDown = true;

	mousePoint.x = x;
	mousePoint.y = y;

	drag.Begin(mousePoint);

	MouseEvent e(MouseEvent::MOUSE_STATE::RBUTTON_DOWN, Point2i(mousePoint.x, mousePoint.y), oldMousePoint);
	Notify(e);
}

void MouseInput::ProcessRButtonUp(int x, int y) {
	rightButtonDown = false;

	drag.End();

	MouseEvent e(MouseEvent::MOUSE_STATE::RBUTTON_UP, Point2i(x, y), oldMousePoint);
	Notify(e);
}


void MouseInput::ProcessMouseMove(int x, int y) {
	if (mousePoint.x == x && mousePoint.y == y)
		return;

	int dragX = x;
	int dragY = y;

	mousePoint.x = dragX;
	mousePoint.y = dragY;

	drag.Track(mousePoint);

	Point2i newMousePoint = mousePoint;
	if (drag.IsTracked()) {

	}
		
	MouseEvent e(MouseEvent::MOUSE_STATE::MOUSE_MOVE, newMousePoint, oldMousePoint);
	Notify(e);

	oldMousePoint = newMousePoint;
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

MouseInput::MouseEvent::MouseEvent(MOUSE_STATE _mouseState, const Point2i& _newMousePoint, const Point2i& _oldMousePoint) {
	newMousePoint = _newMousePoint;
	oldMousePoint = _oldMousePoint;

	mouseState = _mouseState;
}
const MouseInfo& MouseInput::MouseEvent::GetInfo() {
	return MouseInfo();
}
