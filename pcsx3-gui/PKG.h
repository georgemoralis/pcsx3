#pragma once
#include <vector>
#include "types.h"
#include <io.h>
#include <windows.h>

enum PKGRevision : u16 {
	PKG_REVISION_DEBUG = 0x0000,
	PKG_REVISION_RELEASE = 0x8000,
};

enum PKGType : u16 {
	PKG_TYPE_PS3 = 0x0001,  // PlayStation 3
	PKG_TYPE_PSP = 0x0002,  // PlayStation Portable
};

struct PKGHeader {
	/*BE*/u32 magic;                  // Magic
	/*BE*/u16 pkg_revision;           // Revision
	/*BE*/u16 pkg_type;               // Type
	/*BE*/u32 pkg_info_offset;        // Info offset
	/*BE*/u32 pkg_info_count;         // Info count
	/*BE*/u32 header_size;            // Header size (usually 0xC0)
	/*BE*/u32 item_count;             // Files and folders in the encrypted data
	/*BE*/u64 total_size;             // Total PKG file size
	/*BE*/u64 data_offset;            // Encrypted data offset
	/*BE*/u64 data_size;              // Encrypted data size
	s8 contentid[48];               // Content ID (similar to "XX####-XXXX#####_##-XXXXXXXXXXXX####")
	s8 digest[16];                  // SHA1 hash from files and attributes
	s8 klicensee[16];               // AES-128-CTR IV
	s8 header_cmac_hash[16];        // CMAC OMAC hash of [0x00, 0x7F]
	s8 header_npdrm_signature[40];  // Header NPDRM ECDSA (R_sig, S_sig)
	s8 header_sha1_hash[8];         // Last 8 bytes of SHA1 of [0x00, 0x7F]
};
inline void ReadBE(PKGHeader & s)
{
	ReadBE(s.magic);
	ReadBE(s.pkg_revision);
	ReadBE(s.pkg_type);
	ReadBE(s.pkg_info_offset);
	ReadBE(s.pkg_info_count);
	ReadBE(s.header_size);
	ReadBE(s.item_count);
	ReadBE(s.total_size);
	ReadBE(s.data_offset);
	ReadBE(s.data_size);
}

struct PKGEntry {
	/*BE*/u32 name_offset;
	/*BE*/u32 name_size;
	/*BE*/u64 data_offset;
	/*BE*/u64 data_size;
	/*BE*/u32 type;
	U32 padding;
};
inline void ReadBE(PKGEntry & s)
{
	ReadBE(s.name_offset);
	ReadBE(s.name_size);
	ReadBE(s.data_offset);
	ReadBE(s.data_size);
	ReadBE(s.type);
}

struct PKGFooter
{
	s8 sha1[20];//size??
};

class PKG
{
private:
	U08* pkg;
	U64 pkgSize = 0;
	S08 pkgSHA1[20];
	S08 pkgTitleID[9];
	std::string extractPath;
	U64 offset;
	U32 n_files;
public:
	PKG();
	~PKG();
	bool open(const std::string& filepath);
	U64  getPkgSize();
	S08*  getPkgSHA1();
	std::string getTitleID();
	bool extract(const std::string& filepath, const std::string& extractPath,std::string& failreason);
	U32 getNumberOfFiles();

	void *mmap(size_t sLength, std::FILE* nFd) {
		HANDLE hHandle;
		void *pStart;
		hHandle = CreateFileMapping(
			(HANDLE)_get_osfhandle(_fileno((nFd))),
			NULL,                    // default security
			PAGE_WRITECOPY,          // read/write access
			0,                       // maximum object size (high-order DWORD)
			0,                // maximum object size (low-order DWORD)
			NULL);                 // name of mapping object

		if (hHandle != NULL) {
			pStart = MapViewOfFile(hHandle, FILE_MAP_COPY, 0, 0, sLength);
		}
		return pStart;
	}
	int munmap(void *pStart) {
		if (UnmapViewOfFile(pStart) != 0)
			return FALSE;

		return TRUE;
	}
};

