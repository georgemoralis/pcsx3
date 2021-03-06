#pragma once
#include "common.h"
#include <map>
#include "fsFile.h"

//Header
typedef struct PUPHeader
{
	u64 magic;
    u64 package_version;
    u64 image_version;
    u64 file_count;
    u64 header_length;
    u64 data_length;
} PUPHeader;

inline void ReadBE(PUPHeader & s)
{
    ReadBE(s.magic);
    ReadBE(s.package_version);
    ReadBE(s.image_version);
    ReadBE(s.file_count);
    ReadBE(s.header_length);
    ReadBE(s.data_length);
}

//File Entry 
typedef struct PUPFileEntry
{
	U64 entry_id;
	U64 data_offset;
	U64 data_length;
	U08 padding[8];
} PUPFileEntry;

inline void ReadBE(PUPFileEntry & s)
{
    ReadBE(s.entry_id);
    ReadBE(s.data_offset);
    ReadBE(s.data_length);
}

//Hash Entry 
typedef struct PUPHashEntry
{
	U64 entry_id;
	U08 hash[20];
	U08 padding[4];
} PUPHashEntry;

inline void ReadBE(PUPHashEntry & s)
{
    ReadBE(s.entry_id);
}

//Footer
typedef struct PUPFooter
{
	U08 hash[20];
	U08 padding[12];
} PUPFooter;

inline void ReadBE(PUPFooter & s)
{
}

//Entry Id 
struct PUPEntryID
{
	U64 id;
	const char *filename;
};

//PUPEntryID entries 
static struct PUPEntryID entries[] = {
	{ 0x100, "version.txt" },
	{ 0x101, "license.xml" },
	{ 0x102, "promo_flags.txt" },
	{ 0x103, "update_flags.txt" },
	{ 0x104, "patch_build.txt" },
	{ 0x200, "ps3swu.self" },
	{ 0x201, "vsh.tar" },
	{ 0x202, "dots.txt" },
	{ 0x203, "patch_data.pkg" },
	{ 0x300, "update_files.tar" },
	{ 0x501, "spkg_hdr.tar" },
	{ 0x601, "ps3swu2.self" },
	{ 0, NULL }
};


typedef struct 
{                              /* byte offset */
	char name[100];               /*   0 */
	char mode[8];                 /* 100 */
	char uid[8];                  /* 108 */
	char gid[8];                  /* 116 */
	char size[12];                /* 124 */
	char mtime[12];               /* 136 */
	char chksum[8];               /* 148 */
	char typeflag;                /* 156 */
	char linkname[100];           /* 157 */
	char magic[6];                /* 257 */
	char version[2];              /* 263 */
	char uname[32];               /* 265 */
	char gname[32];               /* 297 */
	char devmajor[8];             /* 329 */
	char devminor[8];             /* 337 */
	char prefix[155];             /* 345 */
	//char padding[12];			  /* 500 */
} tar_header;

typedef struct  {
	U32 magic;
	U32 hdr_version;
	U16 key_revision;
	U16 header_type;
	U32 metadata_offset;
	U64 header_length;
	U64 data_length;
}SceHeader;

typedef struct  {
	U08 key[0x10];
	U08 key_pad[0x10];
	U08 iv[0x10];
	U08 iv_pad[0x10];
}MetadataInfo;

typedef struct  {
	U64 signature_input_length;
	U32 unknown1;
	U32 section_count;
	U32 key_count;
	U32 opt_header_size;
	U32 unknown2;
	U32 unknown3;
}MetadataHeader;

typedef struct  {
	U64 data_offset;
	U64 data_size;
	U32 type;
	U32 program_idx;
	U32 hashed;
	U32 sha1_idx;
	U32 encrypted;
	U32 key_idx;
	U32 iv_idx;
	U32 compressed;
}MetadataSectionHeader;

typedef struct
{
	U64 authid;
	U32 vendor_id;
	U32 self_type;
	U64 version;
	U64 padding;
}AppInfo;

class PUP
{
public:
	PUP();
	virtual ~PUP();
	bool Read(const std::string& filepath, const std::string& extractPath);
	int parseoct(const char *p, size_t n);
	std::map<std::string, U64> offset_map;
	U08* decryptpkg(U08 *pkg,U32 &filesize);
	void sce_decrypt_header(U08 *ptr);
	void sce_decrypt_data(U08 *ptr, U08 *extracted);

	//debug info
	void printPUPHeader(PUPHeader puph);
	void printPUPFileEntries(PUPHeader puph, PUPFileEntry fentry);

	const char *id2name(U64 id, struct PUPEntryID *t, const char *unk)
	{
		while (t->filename != NULL) {
			if (id == t->id)
				return t->filename;
			t++;
		}
		return unk;
	}
};

