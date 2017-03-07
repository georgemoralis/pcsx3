#include "fsFile.h"

//basic and probably temporary file handling

fsFile::fsFile()
{
	m_file = nullptr;
}
fsFile::fsFile(const std::string& path, fsOpenMode mode)
{
	Open(path, mode);
}
bool fsFile::Open(const std::string& path, fsOpenMode mode)
{
	Close();
	fopen_s(&m_file, path.c_str(), getOpenMode(mode));
	return IsOpen();
}
bool fsFile::Close()
{
	if (!IsOpen() || std::fclose(m_file) != 0) {
		m_file = nullptr;
		return false;
	}

	m_file = nullptr;
	return true;
}
fsFile::~fsFile()
{
	Close();
}

bool fsFile::Write(const void* src, U64 size)
{
	if (!IsOpen() || std::fwrite(src, 1, size, m_file) != size) {
		return false;
	}
	return true;
}

bool fsFile::Read(void* dst, U64 size)
{
	if (!IsOpen() || std::fread(dst, 1, size, m_file) != size) {
		return false;
	}
	return true;
}
U32 fsFile::ReadBytes(void* dst, U64 size)
{
	return std::fread(dst, 1, size, m_file);
}

bool fsFile::Seek(S64 offset, fsSeekMode mode)
{
	if (!IsOpen() || _fseeki64(m_file, offset, getSeekMode(mode)) != 0) {
		return false;
	}
	return true;
}

U64 fsFile::Tell() const
{
	if (IsOpen()) {
		return _ftelli64(m_file);
	}
	else {
		return -1;
	}
}
U64 fsFile::getFileSize()
{
	U64 pos = _ftelli64(m_file);
	if (_fseeki64(m_file, 0, SEEK_END) != 0) {
		
		return 0;
	}

	U64 size = _ftelli64(m_file);
	if (_fseeki64(m_file, pos, SEEK_SET) != 0) {

		return 0;
	}
	return size;
}

bool fsFile::IsOpen() const
{
	return m_file != nullptr;
}

