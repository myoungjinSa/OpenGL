#pragma once
#include "Types.h"
#include <map>
#include "String/String.h"
#include "KeyCode.h"
#include "Observer.h"
#include "Drag.h"

class KeyInfo : public EventInfo {
public:
	KeyInfo();

	bool bToggleMenu;
	bool bToggleCtrl;
	bool bToggleShift;

	unsigned int key;
};

class KeyboardInput
{
public:
	class KeyboardEvent : public Event {
	public:
		enum class KEY_STATE {
			KEY_STATE_DOWN,
			KEY_STATE_UP,
			KEY_STATE_COUNT
		};

		KeyboardEvent(KEY_STATE keyState, const std::vector<unsigned int>& keys);
		const KeyInfo& GetInfo() override;
		
		std::vector<unsigned int> keys;
		KEY_STATE keyState;
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
	static void Notify(KeyboardInput::KeyboardEvent& e);
private:
	static bool LoadKeyCodes(String&& keyFile);
	static std::map<unsigned int, std::pair<const String, bool>> keyCodes;

	static std::vector<Observer*> observers;
};

class MouseInfo : public EventInfo {

};

class MouseInput
{
public:
	class MouseEvent : public Event {
	public:
		enum class MOUSE_STATE {
			LBUTTON_DOWN,
			LBUTTON_UP,
			RBUTTON_DOWN,
			RBUTTON_UP,
			MOUSE_MOVE,
			MOUSE_STATE_COUNT
		};
		MouseEvent(MOUSE_STATE mouseState, const Point2i& newMousePoint, const Point2i& oldMousePoint);
		const MouseInfo& GetInfo() override;

		Point2i oldMousePoint;
		Point2i newMousePoint;
		MOUSE_STATE mouseState;
	};

	MouseInput();
	MouseInput(const MouseInput& other) = delete;
	~MouseInput();

	static bool Initialize();
	static void ProcessRButtonDown(int x, int y);
	static void ProcessRButtonUp(int x, int y);
	static void ProcessLButtonUp(int x, int y);
	static void ProcessLButtonDown(int x, int y);
	static void ProcessMouseMove(int x, int y);

	static void Attach(Observer* observer);
	static bool Detach(Observer* observer);

	static void Notify(MouseInput::MouseEvent& e);

	static bool leftButtonDown;
	static bool rightButtonDown;

	static Point2i oldMousePoint;
	static Point2i mousePoint;

	static float xAngle;
	static float yAngle;

private:
	static Drag  drag;
	static std::vector<Observer*> observers;
};