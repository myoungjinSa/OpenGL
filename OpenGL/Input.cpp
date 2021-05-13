#include "Input.h"
#include "System.h"

float Input::xAngle = 3.14f;
float Input::yAngle = 0.0f;

Input::Input()
	:	dragging(false),
		mousePoint()
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
	mousePoint.x = x;
	mousePoint.y = y;
}
void Input::ProcessMouseMove(int x, int y) {
	if (dragging) {
		int dragX = x;
		int dragY = y;

		xAngle += (float)(dragY - mousePoint.y) / 3.6f;
		yAngle += (float)(dragX - mousePoint.x) / 3.6f;

		mousePoint.x = dragX;
		mousePoint.y = dragY;
		//dragY = y;
	}
}

void Input::ProcessLButtonUp(int x, int y) {
	if (dragging)
		dragging = false;
}