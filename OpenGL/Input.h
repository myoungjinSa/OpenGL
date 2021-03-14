#pragma once

class Input
{
public:
	Input();
	Input(const Input& other);
	~Input();

	void Initialize();
	void KeyDown(unsigned int key);
	void KeyUp(unsigned int key);

	bool IsKeyDown(unsigned int key);

private:
	bool keys[256];
};

