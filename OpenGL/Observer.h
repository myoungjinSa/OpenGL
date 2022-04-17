#pragma once

class Event abstract {
public:
	virtual void GetInfo() = 0;
};


class Observer{
public:
	virtual void ProcessEvent(Event& e) {};
};
