#include "String.h"
#include <vector>
#include <string.h>
#include <stdarg.h>

//using namespace MJCore;

String::String() :string{ nullptr }, length{ 0 }, currentChar{ 0 }, capacity{ 0 } {}
String::String(char ch) 
	: string{ nullptr }, length{ 0 }, currentChar{ 0 }, capacity{0} {
	string = (char*)malloc(sizeof(char));
	string[0] = ch;
	length = 1;
	capacity = 1;
	currentChar = 0;
}
String::String(const char* _string) 
	:string{ nullptr }, length{ 0 }, currentChar{ 0 }, capacity{0} {
	size_t len = strlen(_string);
	length = len;
	capacity = length + 1;
	string = (char*)malloc(sizeof(char) * capacity);

	for (size_t ch = 0; ch < len; ++ch) {
		string[ch] = _string[ch];
	}
	string[length] = '\0';
	std::cout << "Constructor Called\n";
}


String::~String() {
	free(string);
	string = 0;
}

String::String(String&& other) noexcept {
	std::cout << "Move Constructor Called\n";
	
	string = other.string;
	length = other.length;
	currentChar = other.currentChar;
	capacity = other.capacity;

	other.string = nullptr;
	other.length = 0;
	other.currentChar = 0;
	other.capacity = 0;
}
String& String::operator=(String&& other) noexcept{
	if (this == &other)
		return *this;

	std::cout << "Move operator Called\n";
	string = other.string;
	length = other.length;
	currentChar = other.currentChar;
	capacity = other.capacity;

	other.string = nullptr;
	other.length = 0;
	other.currentChar = 0;
	other.capacity = 0;
	return *this;
}


String::String(const String& other) {
	std::cout << "Copy Constructor Called\n";

	length = other.length;
	capacity = other.capacity;

	string = (char*)malloc(sizeof(char) * capacity);

	memcpy_s(string, length, other.string, length);
	string[length] = '\0';
	
	currentChar = other.currentChar;
}
const String& String::operator=(const String& other) {
	std::cout << "Move Operator Called\n";

	if (this == &other)
		return *this;

	if (string) {
		free(string);
		string = 0;
	}

	length = other.length;
	capacity = other.capacity;
	string = (char*)malloc(sizeof(char) * capacity);

	memcpy_s(string, length, other.string, length);
	string[length] = '\0';

	currentChar = other.currentChar;
	return *this;
}


bool String::Compare(const String& other) const{
	if (length != other.length)
		return false;

	for (size_t iChar = 0; iChar < length; ++iChar) {
		if (string[iChar] != other.string[iChar])
			return false;
	}

	return true;
}

String String::SubString(size_t strPos, size_t count) {
	const size_t tempStringCount = count + 1;

	char* tempString = (char*)malloc(sizeof(char) * (count + 1));
	memcpy_s(tempString, count, &string[strPos], count );
	tempString[count] = '\0';
	
	String retString = tempString;
	free(tempString);
	return retString;
}


bool String::Append(const char* str) {
	size_t tempLength = length;
	length = length + strlen(str);
	capacity = length + 1;
	char* tempString = string;

	string = (char*)realloc(string, sizeof(char) * capacity);

	if (string == nullptr) {
		return false;
	}

	for (size_t iChar = tempLength, iAppendedChar = 0; iChar <= length; ++iChar, ++iAppendedChar) {
		if (iAppendedChar >= strlen(str))
			break;

		string[iChar] = str[iAppendedChar];
	}
	string[length] = '\0';

	return true;
}


bool String::Append(const String& str) {
	size_t tempLength = length;
	length = length + str.length;
	capacity = length + 1;
	char* tempString = string;

	string = (char*)realloc(string, sizeof(char) * capacity);

	if (string == nullptr) {
		return false;
	}

	for (size_t iChar = tempLength, iAppendedChar = 0; iChar <= length; ++iChar, ++iAppendedChar) {
		if (iAppendedChar >= str.Length())
			break;

		string[iChar] = str.string[iAppendedChar];
	}
	string[length] = '\0';

	return true;
}
bool String::Resize(size_t size) {
	char* tempString = nullptr;
	tempString = string;

	length = size;
	capacity = length + 1;
	string = (char*)realloc(string, sizeof(char) * capacity);
	if (string == NULL) {
		return false;
	}
	memset(string, 0, length);

	memcpy_s(string, length, tempString, strlen(tempString));
	string[length] = '\0';

	return true;
}
bool String::Reserve(size_t reserveSize) {
	char* tempString = nullptr;
	capacity = reserveSize;
	string = (char*)realloc(string, sizeof(char) * capacity);
	
	if (!string) {
		string = tempString;
		return false;
	}

	memset(&string[length], 0, capacity - length);
	return true;
}

/*2번째꺼부터 2개의 문자열을 str로대체 Replace(2, 2, str);
abcdefg -> abddddefg	e의 index 위치가 4에서 6이 됨 -> 
			str문자열 길이에서 대체된 문자개수 (count)을 뺀만큼 우측으로 이동
*/
void String::Replace(size_t index, size_t count, const String& str) {
	int size = capacity + str.length - count;
	
	if (Reserve(size)) {
		int offset = str.length - count;
		int moveIndex = index + count;

		//for(int i = moveIndex; i < )
		//for(size_t iChar = size - 1; iChar )
	}
}
void String::Clear() {
	for (size_t iChar = 0; iChar < length; ++iChar) {
		string[iChar] = ' ';
	}

	delete[] string;
	string = nullptr;
	capacity = 0;
	length = 0;
	currentChar = 0;
}

const char* String::c_str() const{
	return &string[0];
}

String String::Format(String fmt, ...) {
	size_t size = fmt.capacity;
	String buffer;
	buffer.Reserve(size);
	va_list arg;
	while (1) {
		buffer.Resize(size);
		va_start(arg, fmt);
		int nPos = vsnprintf((char*)&buffer[0], size, &fmt[0], arg);
		va_end(arg);
		if (nPos > -1 && nPos < size) {
			buffer.Resize(nPos);
			return buffer;
		}
		if (nPos > -1) 
			size = nPos + 1;
		else
			size *= 2;
		
	}
	return buffer;
}

char& String::operator[](size_t index) {
	return string[index];
}

String::iterator String::begin() {
	return iterator(string);
}

String::iterator String::end() {
	return iterator(string + capacity);
}

String::reverse_iterator String::rbegin() {
	return reverse_iterator(string + capacity);
}

String::reverse_iterator String::rend() {
	return reverse_iterator(string);
}
inline size_t String::Length()const {
	return length;
}

inline bool String::operator<(const String& other) {
	size_t iChar = 0;
	while (iChar < length && iChar < other.string[iChar]) {
		if (string[iChar] != other.string[iChar]) {
			break;
		}
		iChar++;
	}
	return string[iChar] < other.string[iChar];
}
bool String::operator!=(const String& other) {
	return !Compare(other);
}
bool String::operator==(const String& other) {
	return Compare(other);
}

bool String::Empty() const {
	return length == 0;
}
//////////////////////////////////////////////////////////////
std::ostream& operator<<(std::ostream& os, const String& str) {
	for (char& ch : str)
		os << ch;
	return os;
}

int StringToInt(String str) {
	int value = 0;
	int positive = 1;

	if (*str.string == '\0')
		return 0;

	if (*str.string == '-')
		positive = -1;

	String::iterator iter = str.begin();

	while (iter != str.end()) {
		if ('0' <= (*iter) && (*iter) <= '9') {
			value = value * 10 + (*iter) - '0';
		}
		++iter;
	}

	return	value * positive;
}

//int StringToInt(String str) {
//
//	size_t lastIndexWithNumber = 0;
//	for (size_t iChar = 0; iChar < str.Length(); iChar++) {
//		if (!isdigit(atoi(&str[iChar])))
//			break;
//
//	}
//	String s;
//	s.Reserve(lastIndexWithNumber);
//	for (size_t iIndex = 0; iIndex < lastIndexWithNumber; iIndex++) {
//		s.Append(str[iIndex]);
//	}
//
//	return atoi(s.c_str());
//}


String IntToString(int num) {
	String s;
	char* numStr = nullptr;
	
	int count = 0;
	int loop = num;
	while (loop >= 1) {
		loop = loop / 10;
		count++;
	}
	numStr = (char*)malloc(sizeof(char) * count + 1);
	memset(numStr, 0, sizeof(char) * count + 1);

	for (int iChar = count - 1; iChar >= 0; --iChar) {
		char ch = 0;
		switch (num % 10) {
		case 0:		ch = '0';	break;
		case 1:		ch = '1';	break;
		case 2:		ch = '2';	break;
		case 3:		ch = '3';	break;
		case 4:		ch = '4';	break;
		case 5:		ch = '5';	break;
		case 6:		ch = '6';	break;
		case 7:		ch = '7';	break;
		case 8:		ch = '8';	break;
		case 9:		ch = '9';	break;
		default:				
			break;
		}
		numStr[iChar] = ch;
		num /= 10;
	}
	numStr[count] = '\0';


	s.Append(numStr);
	return s;
}

std::vector<String> Split(const String& targetString, char token) {
	std::vector<String> ret;
	String::iterator iter = targetString.begin();
	String::iterator tokenIter = targetString.begin();
	String keyComponents;
	while (iter != targetString.end()) {
		if ((*iter) == token) {
			auto dist = std::distance(tokenIter, iter);
			keyComponents.Reserve(dist);
			for (String::iterator it = tokenIter; it != iter; ++it) {
				keyComponents.Append(*it);
			}
			ret.emplace_back(keyComponents);
			tokenIter = iter + 1;
		}	
		iter++;
	}
	keyComponents.Clear();
	for (String::iterator it = tokenIter; it != iter; ++it) {
		keyComponents.Append(*it);
	}
	ret.emplace_back(keyComponents);
	
	return ret;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//WString
WString::WString() : wstring{ nullptr }, currentChar{ 0 }, length{ 0 }, capacity{ 0 } {

}

WString::WString(wchar_t ch)
	: wstring{ nullptr }, currentChar{ 0 }, length{ 0 }, capacity{ 0 }{
	wstring = (wchar_t*)malloc(sizeof(wchar_t));
	wstring[0] = ch;
	length = 1;
	capacity = 1;
	currentChar = 0;
}

WString::WString(const wchar_t* _wstring)
	: wstring{ nullptr }, currentChar{ 0 }, length{ 0 }, capacity{ 0 }{
	size_t len = wcslen(_wstring);
	length = len;
	capacity = length + 1;
	wstring = (wchar_t*)malloc(sizeof(wchar_t) * capacity);

	for (size_t ch = 0; ch < len; ++ch) {
		wstring[ch] = _wstring[ch];
	}
	wstring[length] = '\0';
	std::cout << "Constructor Called\n";
}

WString::WString(const WString& other) {
	std::cout << "Copy Constructor Called\n";

	length = other.length;
	capacity = other.capacity;

	wstring = (wchar_t*)malloc(sizeof(wchar_t) * capacity);

	memcpy_s(wstring, sizeof(wchar_t) * length, other.wstring, sizeof(wchar_t) * length);
	wstring[length] = '\0';

	currentChar = other.currentChar;
}

const WString& WString::operator=(const WString& other) {
	std::cout << "Copy Operator Called\n";

	if (this == &other)
		return *this;

	if (wstring) {
		free(wstring);
		wstring = 0;
	}

	length = other.length;
	capacity = other.capacity;
	wstring = (wchar_t*)malloc(sizeof(wchar_t) * capacity);

	memcpy_s(wstring, sizeof(wchar_t) * length, other.wstring, sizeof(wchar_t) * length);
	wstring[length] = '\0';

	currentChar = other.currentChar;
	return *this;
}


WString::WString(WString&& other) noexcept {
	std::cout << "Move Constructor Called\n";

	wstring = other.wstring;
	length = other.length;
	currentChar = other.currentChar;
	capacity = other.capacity;

	other.wstring = nullptr;
	other.length = 0;
	other.currentChar = 0;
	other.capacity = 0;
}
WString& WString::operator=(WString&& other) noexcept {
	if (this == &other)
		return *this;

	std::cout << "Move operator Called\n";
	wstring = other.wstring;
	length = other.length;
	currentChar = other.currentChar;
	capacity = other.capacity;

	other.wstring = nullptr;
	other.length = 0;
	other.currentChar = 0;
	other.capacity = 0;
	return *this;
}

WString::~WString() {
	free(wstring);
	wstring = 0;
	length = 0;
	capacity = 0;
	currentChar = 0;
}


WString::reverse_iterator WString::rbegin()const{
	return reverse_iterator(wstring + capacity);
}

WString::reverse_iterator WString::rend()const{
	return reverse_iterator(wstring);
}

WString::iterator WString::begin()const{
	return iterator(wstring);
}

WString::iterator WString::end()const{
	return iterator(wstring + capacity);
}

bool WString::Compare(const WString& other) const {
	if (length != other.length)
		return false;

	for (size_t iChar = 0; iChar < length; ++iChar) {
		if (wstring[iChar] != other.wstring[iChar])
			return false;
	}

	return true;
}

bool WString::CompareNoCase(const WString& other) const {
	if (length != other.length)
		return false;

	WString toLower(wstring);
	WString toLower2(other);

	int err = _wcslwr_s(const_cast<wchar_t*>(toLower.c_str()), toLower.capacity);
	if (err != 0) {
		return false;
	}
	err = _wcslwr_s(const_cast<wchar_t*>(toLower2.c_str()), toLower2.capacity);
	if (err != 0) {
		return false;
	}

	if (!toLower.Compare(toLower2)) {
		return false;
	}

	WString toUpper(wstring);
	WString toUpper2(other);
	err = _wcsupr_s(const_cast<wchar_t*>(toUpper.c_str()), toUpper.capacity);
	if (err != 0) {
		return false;
	}
	err = _wcsupr_s(const_cast<wchar_t*>(toUpper2.c_str()), toUpper2.capacity);
	if (err != 0) {
		return false;
	}

	if (!toUpper.Compare(toUpper2)) {
		return false;
	}

	return true;
}

void WString::Clear() {
	for (size_t iChar = 0; iChar < length; ++iChar) {
		wstring[iChar] = ' ';
	}

	delete[] wstring;
	wstring = nullptr;
	length = 0;
	currentChar = 0;
	capacity = 0;
}
bool WString::Resize(size_t size) {
	wchar_t* tempString = nullptr;
	bool doCopy = false;
	if (wstring != nullptr) {
		tempString = wstring;
		doCopy = true;
	}
	length = size;
	capacity = length + 1;
	wstring = (wchar_t*)realloc(wstring, sizeof(wchar_t) * capacity);
	if (wstring == NULL) {
		return false;
	}
	
	memset(wstring, 0, sizeof(wchar_t) * capacity);

	if (doCopy) {
		memcpy_s(wstring, sizeof(wchar_t) * capacity, tempString, sizeof(wchar_t) * capacity);
	}
	wstring[length] = '\0';
	return true;
}

bool WString::Reserve(size_t reserveSize) {
	wchar_t* tempString = nullptr;
	capacity = reserveSize;
	length = (0 < (capacity - 1)) ? capacity - 1 : 0;
	wstring = (wchar_t*)realloc(wstring, sizeof(wchar_t) * capacity);

	if (!wstring) {
		wstring = tempString;
		return false;
	}

	memset(&wstring[length], 0, capacity - length);
	return true;
}

WString::reverse_iterator WString::ReverseFind(wchar_t wch) const {
	return ReverseFind(WString(wch));
}
WString::reverse_iterator WString::ReverseFind(const WString& wstr)const {
	return ReverseFind(wstr.c_str());
}
WString::reverse_iterator WString::ReverseFind(const wchar_t* _wstr)const {
	if (wcsnlen(_wstr, 256) == 0) {
		return rend();
	}

	wchar_t* pFind = wcsstr(wstring, _wstr);
	if (!pFind) {
		return rend();
	}
	return WString::reverse_iterator(pFind);
}
WString::iterator WString::Find(wchar_t wch) const {
	return Find(WString(wch));
}
WString::iterator WString::Find(const WString& wstr)const{
	return Find(wstr.c_str());
}

WString::iterator WString::Find(const wchar_t* _wstr)const{
	if (wcsnlen(_wstr, 256) == 0) {
		return end();
	}

	wchar_t* pFind = wcsstr(wstring, _wstr);
	if (!pFind) {
		return end();
	}

	return WString::iterator(pFind);
}

bool WString::Assign(const wchar_t* _wstr) {
	WString wstr(_wstr);
	return Assign(wstr);
}

bool WString::Assign(const WString& wstr) {
	if (!Resize(wstr.length)) {
		return false;
	}
	for (size_t iChar = 0; iChar < length; iChar++) {
		wstring[iChar] = wstr.wstring[iChar];
	}
	wstring[length] = '\0';

	return true;
}
bool WString::Append(const WString& wstr) {
	size_t tempLength = length;
	length = length + wstr.length;
	capacity = length + 1;
	wchar_t* tempString = wstring;

	wstring = (wchar_t*)realloc(wstring, sizeof(wchar_t) * capacity);

	if (wstring == nullptr) {
		return false;
	}

	for (size_t iChar = tempLength, iAppendedChar = 0; iChar <= length; ++iChar, ++iAppendedChar) {
		if (iAppendedChar >= wstr.Length())
			break;

		wstring[iChar] = wstr.wstring[iAppendedChar];
	}
	wstring[length] = '\0';

	return true;
}

bool WString::Append(const wchar_t* wstr) {
	size_t tempLength = length;
	length = length + wcslen(wstr);
	capacity = length + 1;
	wchar_t* tempString = wstring;

	wstring = (wchar_t*)realloc(wstring, sizeof(wchar_t) * capacity);

	if (wstring == nullptr) {
		return false;
	}

	for (size_t iChar = tempLength, iAppendedChar = 0; iChar <= length; ++iChar, ++iAppendedChar) {
		if (iAppendedChar >= wcslen(wstr))
			break;

		wstring[iChar] = wstr[iAppendedChar];
	}
	wstring[length] = '\0';

	return true;
}
WString WString::SubString(size_t stringPos, size_t count)const {
	const size_t tempStringCount = count + 1;

	wchar_t* tempString = (wchar_t*)malloc(sizeof(wchar_t) * (count + 1));
	memcpy_s(tempString, sizeof(wchar_t) * count, &wstring[stringPos], sizeof(wchar_t) * count);
	tempString[count] = '\0';

	WString retString = tempString;
	free(tempString);
	return retString;
}

size_t WString::Length() const {
	return length;
}

bool WString::Empty() const {
	return length == 0;
}

wchar_t& WString::operator[](size_t index) {
	return wstring[index];
}

bool WString::operator==(const WString& other) {
	return Compare(other);
}

bool WString::operator!=(const WString& other) {
	return !Compare(other);
}

const wchar_t* WString::c_str() const {
	return &wstring[0];
}
const wchar_t* WString::c_str() {
	return &wstring[0];
}

///////////////////////////////////////////////////
std::wostream& operator<<(std::wostream& os, const WString& str) {
	for (wchar_t& ch : str)
		os << ch;
	return os;
}