#pragma once

class Object;

class Component {
public:
	explicit Component(Object* _owner);
	
	virtual void Initialize() {}
	virtual void Update(float deltaTime) {}
	virtual void LateUpdate(float deltaTime) {}
	virtual void Render() {}
public:
	Object* owner;
};

