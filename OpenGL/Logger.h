#pragma once
#include <iostream>
#include <functional>
#include "String/String.h"

template<typename Func>
class Logger {
public:
	Logger(const Func& _func, const WString& _content) 
		:func{_func}, content{_content}
	{

	}

	void operator()() const {
		std::cout << content << std::endl;
		func();
	}
private:
	Func func;
	WString content;
};




