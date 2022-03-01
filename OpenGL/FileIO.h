#pragma once
#include <fstream>
#include "String/String.h"

namespace FileUtils {
	WString GetFileExtent(const WString& filepath);
	int GetFileExtent(const WString& filepath, WString Extent);
	int GetFileExtent(const WString& filepath, wchar_t* pExtent);
	int  GetFileExtent(const WString& filepath, WString Extent);
	bool DoesFileExist(const WString& filepath);
}