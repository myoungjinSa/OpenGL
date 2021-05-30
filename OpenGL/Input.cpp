#include "Input.h"
#include "System.h"
#include <fstream>
#include <sstream>


float Input::xAngle = 0.0f;
float Input::yAngle = 0.0f;
bool Input::dragging = false;
Point2i Input::mousePoint;
bool Input::keys[256];
std::map<unsigned int, const String> Input::keyCodes;
Input::Input()
{

}

Input::~Input() {
	keyCodes.clear();
}

void Input::Initialize() {
	
	LoadKeyCodes("keyCodes.txt");
	// Initialize all the keys to being released and not pressed.
	for (int iKey = 0; iKey < 256; iKey++) {
		keys[iKey] = false;
	}
}


void Input::KeyDown(unsigned int input)
{
	// If a key is pressed then save that state in the key array.
	keys[input] = true;
}


void Input::KeyUp(unsigned int input)
{
	// If a key is released then clear that state in the key array.
	keys[input] = false;
}


bool Input::IsKeyDown(unsigned int key)
{
	// Return what state the key is in (pressed/not pressed).
	return keys[key];
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
	if (dragging) 
		dragging = false;

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

	std::map<unsigned int, const String>::iterator iter = keyCodes.begin();
	while (!keyFile.eof()) {
		char _keyName[128];
		keyFile.getline(_keyName, 128);
		
		String keyName = _keyName;
		std::vector<String> t = Split(keyName, ' ');

	

		//keyCodes.insert(iter, std::make_pair((unsigned int)StringToInt(keyCode), keyName));
	}
	
	return true;
}