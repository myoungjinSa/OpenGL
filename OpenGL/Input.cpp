#include "Input.h"

Input::Input() {

}

Input::Input(const Input& other) {

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