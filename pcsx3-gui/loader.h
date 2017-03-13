#pragma once

enum FileTypes
{
	FILETYPE_UNKNOWN,
	FILETYPE_ELF,
	FILETYPE_SELF,
	FILETYPE_PRX,
	FILETYPE_SPRX,
	FILETYPE_PUP,
	FILETYPE_PKG,
	FILETYPE_PSF
};
FileTypes detectFileType(const std::string& filepath);