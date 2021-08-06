#pragma once
#include "Types.h"
#include <map>
#include "String/String.h"
#include "KeyCode.h"
#include "Observer.h"


class KeyboardInput
{
public:
	class KeyboardEvent : public Event {
	public:
		KeyboardEvent();
		void GetInfo() override;
	};

	KeyboardInput();
	KeyboardInput(const KeyboardInput& other) = delete;
	~KeyboardInput();


	static bool Initialize();
	static void KeyDown(unsigned int key);
	static void KeyUp(unsigned int key);
	static bool IsKeyDown(unsigned int key);

	static void Attach(Observer* observer);
	static bool Detach(Observer* observer);
	static void Notify(KeyboardEvent& e);
private:
	static bool LoadKeyCodes(String&& keyFile);
	static std::map<unsigned int, std::pair<const String, bool>> keyCodes;

	static std::vector<Observer*> observers;
};

class MouseInput
{
public:
	class MouseEvent : public Event {
	public:
		MouseEvent();
		void GetInfo() override;
	};

	MouseInput();
	MouseInput(const MouseInput& other) = delete;
	~MouseInput();

	static bool Initialize();
	static void ProcessLButtonUp(int x, int y);
	static void ProcessLButtonDown(int x, int y);
	static void ProcessMouseMove(int x, int y);

	static float GetXAngle();
	static float GetYAngle();

	static void Attach(Observer* observer);
	static bool Detach(Observer* observer);

	static void Notify(MouseEvent& e);
private:
	static bool dragging;
	static Point2i mousePoint;
	static float yAngle;
	static float xAngle;

	static std::vector<Observer*> observers;
};