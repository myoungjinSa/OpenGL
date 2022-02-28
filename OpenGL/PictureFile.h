#pragma once

class WString;
class Picture;
class PictureFile {
public:
	PictureFile();
	virtual ~PictureFile();

	virtual bool Create(Picture& picture, const WString& filename) { return true; }
	virtual bool Load(Picture& picture, const WString& filename) { return true; }
	virtual bool Save(Picture& picture, const WString& filename) { return true; }

	static bool CreatePicture(Picture& picture, const WString& filename);
};

class GDIPictureFile : public PictureFile{
public:
	GDIPictureFile();
	~GDIPictureFile() override;
	bool Create(Picture& picture, const WString& filename) override;
	bool Load(Picture& picture, const WString& filename) override;
	bool Save(Picture& picture, const WString& filename) override;

private:
	static bool LoadGDIPlus();
	static void UnloadGDIPlus();
	static unsigned __int64 gdiplusToken;
};

