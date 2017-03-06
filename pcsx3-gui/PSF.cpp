#include "PSF.h"
#include "fsFile.h"
#include "types.h"
#include <iostream>
#include <fstream>

PSF::PSF()
{
}


PSF::~PSF()
{
}
bool PSF::open(const std::string& filepath) {
	fsFile file;
	if (!file.Open(filepath, fsRead))
	{
		return false;
	}

	const U64 psfSize = file.getFileSize();
	psf.resize(psfSize);
	file.Seek(0, fsSeekSet);
	file.Read(&psf[0], psfSize);

	// Parse file contents
	const auto& header = (PSFHeader&)psf[0];
	for (U32 i = 0; i < header.entries; i++) {
		const U32 offset = sizeof(PSFHeader) + i * sizeof(PSFEntry);
		const auto& entry = (PSFEntry&)psf[offset];

		std::string key = (char*)&psf[header.table_keys + entry.offset_key];
		if (FromBigEndian(entry.type) == PSFEntry::Type::TEXT_RAW ||
			FromBigEndian(entry.type) == PSFEntry::Type::TEXT_NORMAL) {
			map_strings[key] = (char*)&psf[header.table_data + entry.offset_data];
		}
		if (FromBigEndian(entry.type) == PSFEntry::Type::INTEGER) {
			map_integers[key] = (U32&)psf[header.table_data + entry.offset_data];
		}
	}
	//debug code print all keys
	std::ofstream out;
	out.open("psf.txt", std::fstream::out | std::fstream::app);
	out << "---------------------------------------------" << "\n";
	for (auto stringkey : map_strings)
	{
		out << " " << stringkey.first << " : " << stringkey.second << "\n";
	}
	for (auto integerkey : map_integers)
	{
		out << " " << integerkey.first << " : " << integerkey.second << "\n";
	}	
	out << "---------------------------------------------" << "\n";

	return true;
}
std::string PSF::get_string(const std::string& key) {
	if (map_strings.find(key) != map_strings.end()) {
		return map_strings.at(key);
	}
	return "";
}
U32 PSF::get_integer(const std::string& key)
{
	if (map_strings.find(key) != map_strings.end()) {
		return  std::stoi(map_strings.at(key)); //TODO std::invalid_argument exception if it fails?
	}
	return 0;
}