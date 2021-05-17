#pragma once
#include "Types.h"

class Input
{
public:
	Input();
	Input(const Input& other) = delete;
	~Input();

	static void Initialize();
	static void KeyDown(unsigned int key);
	static void KeyUp(unsigned int key);

	static bool IsKeyDown(unsigned int key);
	static void ProcessLButtonUp(int x, int y);
	static void ProcessLButtonDown(int x, int y);
	static void ProcessMouseMove(int x, int y);

	static float GetXAngle();
	static float GetYAngle();
private:
	static bool keys[256];
	static bool dragging;
	static Point2i mousePoint;
	static float yAngle;
	static float xAngle;
};

