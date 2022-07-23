#pragma once
#include <vector>
 
template<typename T>
class Stack{
public:
	Stack(){}
	Stack(const Stack& other) = delete;
	Stack(Stack&& other) = delete;
	
	bool Empty()const {
		return memory.empty();
	}
	void Clear() {
		memory.clear();
	}

	void Push(T element) {
		memory.push_back(element);
	}
	
	void Pop() {
		memory.pop_back();
	}

	T Peek() {
		return memory.back();
	}

protected:
	std::vector<T> memory;
};


