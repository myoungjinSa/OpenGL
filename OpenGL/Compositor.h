#pragma once

class IComponent {
public:
	bool operator==(const IComponent& other) const{ return this == &other; }
	bool operator==(IComponent& other) const{ return this == &other; }

	bool operator!=(const IComponent& other) const{ return this != &other; }
	bool operator!=(IComponent& other) const{ return this != &other; }

	virtual ~IComponent() {}
};


class ICompositor {
public:
	virtual void Add(IComponent* obj) = 0;
	virtual bool Remove(IComponent* obj) = 0;
};