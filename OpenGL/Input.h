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
		enum class KEY_STATE {
			KEY_STATE_DOWN,
			KEY_STATE_UP,
			KEY_STATE_COUNT
		};
		KeyboardEvent(KEY_STATE keyState, const std::vector<unsigned int>& keys);
		void GetInfo() override;

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
		MouseEvent(MOUSE_STATE mouseState, const Point2i& mousePos);
		void GetInfo() override;

		Point2i mousePoint;
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

	static float GetXAngle();
	static float GetYAngle();

	static void Attach(Observer* observer);
	static bool Detach(Observer* observer);

	static void Notify(MouseInput::MouseEvent& e);

	static Point2i oldMousePoint;
	static bool leftButtonDown;
	static bool rightButtonDown;
	static bool dragging;
private:
	static Point2i mousePoint;
	static float yAngle;
	static float xAngle;

	static std::vector<Observer*> observers;
};