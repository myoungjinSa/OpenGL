#pragma once
#include "Types.h"
#include <map>
#include "String/String.h"

class Input
{
public:
	Input();
	Input(const Input& other) = delete;
	~Input();

	static bool Initialize();
	static void KeyDown(unsigned int key);
	static void KeyUp(unsigned int key);
	static bool IsKeyDown(unsigned int key);
	static void ProcessLButtonUp(int x, int y);
	static void ProcessLButtonDown(int x, int y);
	static void ProcessMouseMove(int x, int y);

	static float GetXAngle();
	static float GetYAngle();

	
private:
	static bool LoadKeyCodes(String&& keyFile);
	
	static std::map<unsigned int, std::pair<const String, bool>> keyCodes;
	static bool dragging;
	static Point2i mousePoint;
	static float yAngle;
	static float xAngle;
};

