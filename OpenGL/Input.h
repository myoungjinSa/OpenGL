#pragma once
#include "Types.h"

class Input
{
public:
	Input();
	Input(const Input& other) = delete;
	~Input();

	void Initialize();
	void KeyDown(unsigned int key);
	void KeyUp(unsigned int key);

	bool IsKeyDown(unsigned int key);
	void ProcessLButtonUp(int x, int y);
	void ProcessLButtonDown(int x, int y);
	void ProcessMouseMove(int x, int y);

	static float yAngle;
	static float xAngle;
private:
	bool keys[256];
	bool dragging;
	Point2i mousePoint;
};

