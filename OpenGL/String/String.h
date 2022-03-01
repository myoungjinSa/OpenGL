#pragma once
#include <iostream>
#include <vector>

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

	StringIterator& operator--() {
		--p;
		return *this;
	}

	StringIterator operator++(int) {
		p++;
		return StringIterator(p);
	}
	
	StringIterator operator--(int) {
		p--;
		return StringIterator(p);
	}

	T& operator*() const {
		return *p;
	}

	ptrdiff_t operator-(const StringIterator& rhs) const {
		return p - rhs.p;
	}

	bool operator<(const StringIterator& rhs) const { return p < rhs.p; }
	bool operator>(const StringIterator& rhs) const { return p > rhs.p; }
	bool operator<=(const StringIterator& rhs) const { return p <= rhs.p; }
	bool operator>=(const StringIterator& rhs) const { return p >= rhs.p; }
	bool operator==(const StringIterator& rhs) const { return p == rhs.p; }

	StringIterator operator+(ptrdiff_t rhs) const { return p + rhs; }
	StringIterator operator-(ptrdiff_t rhs) const { return p - rhs; }
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

template<typename T>
class ReverseStringIterator {
	T* p{ nullptr };
public:
	ReverseStringIterator(T* p) : p{ p } {};
	bool operator!=(const ReverseStringIterator& rhs) const {
		return p != rhs.p;
	}
	bool operator==(const ReverseStringIterator& rhs) const {
		return p == rhs.p;
	}

	ptrdiff_t operator-(const ReverseStringIterator& rhs) const {
		return p - rhs.p;
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
	friend std::vector<String> Split(const String& targetString, char token);
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
	bool				Assign(const char* str);
	bool				Append(const String& str);
	bool				Append(const char* str);
	String				Format(String fmt, ...);
	bool				Resize(size_t size);
	bool				Reserve(size_t reserveSize);
	void				Clear();  
	const char*			c_str() const;

	bool				operator<(const String& other);
	char&				operator[](size_t index);
	bool				operator!=(const String& other);
	bool				operator==(const String& other);
	String&				operator=(String&& other) noexcept;
	const String&		operator=(const String& other);

	using iterator = StringIterator<char>;
	using reverse_iterator = ReverseStringIterator<char>;

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

std::vector<String> Split(const String& targetString, char token);

//int	StringToInt(String str);

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
	using iterator = StringIterator<wchar_t>;
	using reverse_iterator = ReverseStringIterator<wchar_t>;

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

	reverse_iterator    ReverseFind(wchar_t wch)const;
	reverse_iterator	ReverseFind(const wchar_t* wstr)const;
	reverse_iterator    ReverseFind(const WString& wstr)const;
	iterator			Find(const WString& wstr)const;
	iterator			Find(const wchar_t* wstr)const;
	iterator			Find(wchar_t wch)const;
	bool				Assign(const wchar_t* wstr);
	bool				Assign(const WString& wstr);
	bool				Append(const WString& str);
	bool				Append(const wchar_t* str);
	WString				SubString(size_t stringPos, size_t count)const;
	bool				Compare(const WString& other) const;
	bool				CompareNoCase(const WString& other) const;
	size_t				Length() const;
	bool				Empty()const;
	bool				Resize(size_t size);
	bool				Reserve(size_t reserveSize);
	const wchar_t*		c_str() const;
	const wchar_t*		c_str();
	void				Clear();

	iterator begin()const;
	iterator end()const;
	reverse_iterator rbegin()const;
	reverse_iterator rend()const;
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
