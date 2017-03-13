#include "common.h"
#include "fsFile.h"
#include "loader.h"
FileTypes detectFileType(const std::string& filepath)
{
	if (filepath.size() == 0)//no file loaded
	{
		return FILETYPE_UNKNOWN;
	}
	fsFile file(filepath, fsRead);
	file.Seek(0, fsSeekSet);
	U32 magic;
	file.Read(&magic, sizeof(magic));
	file.Close();
	U32 magicLE = FromBigEndian(magic);
	switch (magicLE)
	{
	case 0x7F454C46: //ELF file non encrypted
		return FILETYPE_ELF;
	case 0x53434555: //PUP file
		return FILETYPE_PUP;
	case 0x7F504B47://PKG
		return FILETYPE_PKG;
	}
	return FILETYPE_UNKNOWN;

}
