#pragma once
#include "common.h"
#include <string>

enum fsOpenMode
{
	fsRead = 0x1,
	fsWrite = 0x2,
	fsReadWrite = fsRead | fsWrite
};

enum fsSeekMode
{
	fsSeekSet,
	fsSeekCur,
	fsSeekEnd,
};

class fsFile
{
	std::FILE* m_file;
public:
	fsFile();
	fsFile(const std::string& path, fsOpenMode mode = fsRead);
	bool Open(const std::string& path, fsOpenMode mode = fsRead);
	bool IsOpen() const;
	bool Close();
	bool Read(void* dst, U64 size);
	bool Write(const void* src, U64 size);
	bool Seek(S64 offset, fsSeekMode mode);
	U64 getFileSize();
	U64 Tell() const;
	~fsFile();

	const char* getOpenMode(fsOpenMode mode)
	{
		switch (mode) {
		case fsRead:        return "rb";
		case fsWrite:       return "wb";
		case fsReadWrite:   return "r+b";
		}

		return "r";
	}

	const int getSeekMode(fsSeekMode mode)
	{
		switch (mode) {
		case fsSeekSet:  return SEEK_SET;
		case fsSeekCur:  return SEEK_CUR;
		case fsSeekEnd:  return SEEK_END;
		}

		return SEEK_SET;
	}
};

