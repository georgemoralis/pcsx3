#pragma once

#include <unordered_map>
#include <vector>
#include "types.h"
struct PSFHeader {
	U32 magic;  //big endian
	U32 version;
	U32 table_keys;
	U32 table_data;
	U32 entries;
};

struct PSFEntry {
	enum Type : U16 {
		TEXT_RAW = 0x0400, // String in UTF-8 format and not NULL terminated
		TEXT_NORMAL = 0x0402, // String in UTF-8 format and NULL terminated
		INTEGER = 0x0404, // Unsigned 32-bit integer
	};

	U16 offset_key;
	U16 type; //big endian
	U32 size;
	U32 capacity;
	U32 offset_data;
};

class PSF
{
	std::vector<U08> psf;
	std::unordered_map<std::string, std::string> map_strings;
	std::unordered_map<std::string, U32> map_integers;

public:
	PSF();
	~PSF();
	void open(const std::string& filepath);

	// Access data
	std::string get_string(const std::string& key);
	U32 get_integer(const std::string& key);
};

