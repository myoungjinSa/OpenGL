#pragma once

class EventInfo {

};
class Event abstract {
public:
	virtual const EventInfo& GetInfo() = 0;
};


class Observer{
public:
	virtual void ProcessEvent(Event& e) {};
};
