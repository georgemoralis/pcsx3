#pragma once
#include "common.h"
#include <map>
//Header
typedef struct {
	U64 magic;
	U64 package_version;
	U64 image_version;
	U64 file_count;
	U64 header_length;
	U64 data_length;
} PUPHeader;

//File Entry 
typedef struct {
	U64 entry_id;
	U64 data_offset;
	U64 data_length;
	U08 padding[8];
} PUPFileEntry;

//Hash Entry 
typedef struct {
	U64 entry_id;
	U08 hash[20];
	U08 padding[4];
} PUPHashEntry;

//Footer
typedef struct
{
	U08 hash[20];
	U08 padding[12];
} PUPFooter;

//Entry Id 
typedef struct {
	U64 id;
	const char *filename;
} PUPEntryID;


//PUPEntryID entries 
static const PUPEntryID entries[] = {
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
	{ 0, "" }
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

class PUP
{
public:
	PUP();
	virtual ~PUP();
	bool Read(const std::string& filepath);
	int parseoct(const char *p, size_t n);
	std::map<std::string, U64> offset_map;
};

