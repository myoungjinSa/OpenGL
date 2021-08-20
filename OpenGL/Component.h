#pragma once

class Object;

class Component {
public:
	explicit Component(Object* _owner);
	
	virtual ~Component() {}
public:
	Object* pOwner;
};

