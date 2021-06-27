#pragma once

class IComponent {
	bool operator==(const IComponent& other) const{ return this == &other; }
	bool operator==(IComponent& other) const{ return this == &other; }

	bool operator!=(const IComponent& other) const{ return this != &other; }
	bool operator!=(IComponent& other) const{ return this != &other; }
};


class ICompositor {
public:
	virtual void Add(IComponent& obj) = 0;
	virtual bool Remove(IComponent& obj) = 0;
	virtual IComponent* GetChild(size_t index) = 0;
};