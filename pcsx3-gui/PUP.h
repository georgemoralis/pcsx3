#pragma once
#include "common.h"

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

class PUP
{
public:
	PUP();
	virtual ~PUP();
	bool Read(const std::string& filepath);
};

