#include "FileIO.h"
#include <assert.h>


namespace FileUtils {
	int GetFileExtent(const WString& filepath, WString Extent) {
		if (filepath.Empty())
			return -1;
		
		WString::reverse_iterator iFind = filepath.ReverseFind(Extent);
		if(iFind == filepath.rend()){
			return -1;
		}
		ptrdiff_t index = iFind - filepath.rend();
		return (int)index;
	}
	int GetFileExtent(const WString& filepath, wchar_t* pExtent) {
		return GetFileExtent(filepath, WString(pExtent));
	}
	WString GetFileExtent(const WString& filepath) {
		WString Extent;
		if (filepath.Empty())
			return Extent;

		int dotIndex = GetFileExtent(filepath, L".");
		if (dotIndex < 0) {
			return Extent;
		}
		
		int BackSlashIndex;
		BackSlashIndex = GetFileExtent(filepath, L"\\");
		if (0 < BackSlashIndex && dotIndex < BackSlashIndex) {
			//폴더명에 '.'이 있는 경우
			return Extent;
		}
		Extent = filepath.SubString(dotIndex, size_t(filepath.Length() - dotIndex));
		assert(!Extent.Empty());
		return Extent;
	}

	bool DoesFileExist(const WString& filepath) {
		WString Extent = GetFileExtent(filepath);
		if (Extent.Empty())
			return false;

		std::ifstream fin;
		fin.open(filepath.c_str());
		if (!fin.is_open()) {
			return false;
		}

		return true;
	}
}