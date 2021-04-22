#pragma once
#include <iostream>

template<typename T>
class StringIterator {
	T* p{ nullptr };
public:
	StringIterator(T* p) : p{ p } {};
	bool operator!= (const StringIterator& rhs) const {
		return p != rhs.p;
	}
	StringIterator& operator++() {
		++p;
		return *this;
	}
	
	T& operator*() const {
		return *p;
	}

	ptrdiff_t operator-(const StringIterator rhs) const {
		return p - rhs.p;
	}

	bool operator<(const StringIterator& rhs) const { return p < rhs.p; }
	bool operator>(const StringIterator& rhs) const { return p > rhs.p; }
	bool operator<=(const StringIterator& rhs) const { return p <= rhs.p; }
	bool operator>=(const StringIterator& rhs) const { return p >= rhs.p; }
	bool operator==(const StringIterator& rhs) const { return p == rhs.p; }

	StringIterator operator+(ptrdiff_t rhs) const { return p + rhs; }
	StringIterator operator-(ptrdiff_t rhs) const { return p - rhs; }
	StringIterator operator--() { --p; return *this; }
};

//String도 iterator_traits 클래스에 " 나도 반복자이고 나는 랜덤반복자입니다. " 라고 자신의 정보를 등록
template<>
struct std::iterator_traits<StringIterator<char>> {
	using iterator_category = random_access_iterator_tag;
	using value_type		= char;
	using difference_type	= ptrdiff_t;
	using pointer			= char*;
	using reference			= char&;
};

class ReverseStringIterator {
	char* p{ nullptr };
public:
	ReverseStringIterator(char* p) : p{ p } {};
	bool operator!=(const ReverseStringIterator& rhs) const {
		return p != rhs.p;
	}

	ReverseStringIterator& operator++() {
		--p;
		return *this;
	}
	char operator*() {
		return *(p - 1);
	}
};


class String 
{
	friend std::ostream& operator<<(std::ostream& os, const String& str);
	friend int StringToInt(String str);
public:
	String();
	String(char ch);
	String(const char* string);
	String(const String& other);
	
	String(String&& other) noexcept;
	~String();

	const char* cbegin() const {
		return &string[0];
	}
	const char* cend() const {
		return &(string[length]);
	}

	char* begin() const{
		return &string[0];
	}
	char* end() const{
		return &(string[length]);
	}

	char& operator++() {
		if (currentChar < length - 1)
			currentChar++;

		return string[currentChar];
	}

	char& operator*() {
		return string[0];
	}

	size_t				Length() const;
	bool				Empty()const;
	bool				Compare(const String& other) const;
	String				SubString(size_t stringPos, size_t count);
	void				Replace(size_t index, size_t count, const String& str);
	bool				Append(const String& str);
	bool				Append(const char* str);
	String				Format(String fmt, ...);
	bool				Resize(size_t size);
	bool				Reserve(size_t reserveSize);
	void				Clear();  
	const char*			c_str() const;

	char&				operator[](size_t index);
	bool				operator!=(const String& other);
	bool				operator==(const String& other);
	String&				operator=(String&& other) noexcept;
	const String&		operator=(const String& other);

	using iterator = StringIterator<char>;
	using reverse_iterator = ReverseStringIterator;

	iterator			begin();
	iterator			end();
	reverse_iterator	rbegin();
	reverse_iterator	rend();

private:
	char*				string;
	size_t				currentChar;
	size_t				length;
	int					capacity;
};


template<typename ...Args>
String Format(const String& format, Args ...args) {
	size_t size = snprintf(nullptr, 0, format.begin(), args...) + 1;
	std::unique_ptr<char[]> buffer(new char[size]);
	snprintf(buffer.get(), size, format.begin(), args...);
	return String(buffer.get());
}
String IntToString(int num);


//////////////////////////////////////////////////////////////////////////////////

template<>
struct std::iterator_traits<StringIterator<wchar_t>> {
	using iterator_category = random_access_iterator_tag;
	using value_type = wchar_t;
	using difference_type = ptrdiff_t;
	using pointer = wchar_t*;
	using reference = wchar_t&;
};

class WString {
	friend std::wostream& operator<<(std::wostream& os, const WString& str);
public:
	WString();
	WString(wchar_t ch);
	WString(const wchar_t* wstring);
	WString(const WString& other);
	WString(WString&& other) noexcept;
	~WString();

	wchar_t& operator++() {
		if (currentChar < length - 1)
			currentChar++;

		return wstring[currentChar];
	}

	wchar_t& operator*() {
		return wstring[0];
	}

	const WString&		operator=(const WString& other);
	WString&			operator=(WString&& other) noexcept;

	wchar_t&			operator[](size_t index);
	bool				operator!=(const WString& other);
	bool				operator==(const WString& other);

	bool				Append(const WString& str);
	bool				Append(const wchar_t* str);
	WString				SubString(size_t stringPos, size_t count);
	bool				Compare(const WString& other) const;
	size_t				Length() const;
	bool				Empty()const;
	bool				Resize(size_t size);
	bool				Reserve(size_t reserveSize);

	void				Clear();

	wchar_t* begin() const {
		return &wstring[0];
	}
	wchar_t* end() const {
		return &(wstring[length]);
	}

	using iterator = StringIterator<wchar_t>;

	iterator begin();
	iterator end();
private:
	wchar_t*	wstring;
	size_t		currentChar;
	size_t		length;
	int			capacity;
};

template<typename ...Args>
WString Format(const WString& format, Args ...args) {
	size_t size = swprintf(nullptr, 0, format.begin(), args...) + 1;
	std::unique_ptr<wchar_t[]> buffer(new wchar_t[size]);
	swprintf(buffer.get(), size, format.begin(), args...);
	return WString(buffer.get());
}
