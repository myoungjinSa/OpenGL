#include "Input.h"
#include "System.h"

float Input::xAngle = 0.0f;
float Input::yAngle = 0.0f;
bool Input::dragging = false;
Point2i Input::mousePoint;
bool Input::keys[256];

Input::Input()
{

}

Input::~Input() {

}

void Input::Initialize() {
	
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