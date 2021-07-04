#include "Input.h"
#include "System.h"
#include "Logger.h"
#include <fstream>
#include <sstream>


float Input::xAngle = 0.0f;
float Input::yAngle = 0.0f;
bool Input::dragging = false;
Point2i Input::mousePoint;
std::map<unsigned int, std::pair<const String, bool>> Input::keyCodes;
Input::Input()
{

}

Input::~Input() {
	keyCodes.clear();
}

bool Input::Initialize() {
	return LoadKeyCodes("keyCodes.txt");
}


void Input::KeyDown(unsigned int input)
{
	keyCodes[input].second = true;
}


void Input::KeyUp(unsigned int input)
{
	keyCodes[input].second = false;
}


bool Input::IsKeyDown(unsigned int input)
{
	// Return what state the key is in (pressed/not pressed).
	return keyCodes[input].second;
}


void Input::ProcessLButtonDown(int x, int y) {
	dragging = !dragging;
	if (dragging) {
		mousePoint.x = x;
		mousePoint.y = y;
	}
}
void Input::ProcessMouseMove(int x, int y) {
	if (dragging) {
		int dragX = x;
		int dragY = y;

		xAngle += (y - mousePoint.y) / 3.6;
		yAngle += (x - mousePoint.x) / 3.6;

		mousePoint.x = dragX;
		mousePoint.y = dragY;
		//dragY = y;
	}
}

void Input::ProcessLButtonUp(int x, int y) {
	if (dragging) {
		dragging = false;
	}
}

float Input::GetXAngle(){
	return xAngle;
}

float Input::GetYAngle(){
	return yAngle;
}

bool Input::LoadKeyCodes(String&& keyFileName) {
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