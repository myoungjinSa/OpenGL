#pragma once
#include "Common.h"

class Observer abstract {
public:
	virtual void Listen() = 0;
};

class Observable {
public:
	void Attach(const std::shared_ptr<Observer>& observer) {
		observers.push_back(observer);
	}
	bool Detach(const std::shared_ptr<Observer>& observer) {
		observers.erase(std::remove_if(observers.begin(), observers.end(), [&observer](const std::shared_ptr<Observer>& ob) {
			if (ob == observer)
				return true;
			else
				return false;
			}));
		/*observers.erase(std::remove_if(observers.begin(), observers.end(), [&](const std::shared_ptr<Observer>& ob){
			auto s = std::find_if(observers.begin(), observers.end(), [&](const std::shared_ptr<Observer>& ob) {
				if (observer == ob && observer != nullptr) {
					return true;
				}
				else {
					return false;
				}
				});

			return s;
		}), observers.end());*/
	}

	void Notify();
protected:
	std::vector<std::shared_ptr<Observer>> observers;
};