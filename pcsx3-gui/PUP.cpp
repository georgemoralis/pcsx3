#include "types.h"
#include "PUP.h"
#include "fsFile.h"

using namespace std;

PUP::PUP()
{

}
PUP::~PUP()
{

}
bool PUP::Read(const std::string& filepath)
{
	fsFile file;
	file.Open(filepath, fsRead);
	PUPHeader pupheader;
	file.Read(&pupheader, sizeof(pupheader));
	if (FromBigEndian(pupheader.magic) >> 32 != 0x53434555)//magic is not correct
	{
		file.Close();
		return false;
	}
	else
	{
		U64 filenumber = FromBigEndian(pupheader.file_count);
		for (int i = 0; i < pupheader.file_count; i++)
		{
			file.Seek(0x30 + (0x20 * i), fsSeekSet);
			PUPFileEntry fentry;
			file.Read(&fentry, sizeof(fentry));
			if (FromBigEndian(fentry.entry_id) == 0x300)
			{
				U64 length = FromBigEndian(fentry.data_length);
				U64 offset = FromBigEndian(fentry.data_offset);
				fsFile tar;
				
				char *f = new char[length];
				file.Seek(offset, fsSeekSet);
				file.Read(f, length);
				int off = 0;
				for (;;)
				{
					tar_header& tarh = (tar_header&)f[off];
					int filesize = parseoct(tarh.size, 12);
					if (tarh.typeflag == '0')
					{
						offset_map[tarh.name] = off;
					}
					off += ((filesize+sizeof(tar_header) + 512 - 1) & ~(512 - 1));//alligned to 512 bytes
					if (off > length)
						break;
				}
				for (auto it = offset_map.cbegin(); it != offset_map.cend(); ++it)
				{
					if (it->first.find("dev_flash_") == 0)//extract only devflash files for now
					{
						tar_header& tarh = (tar_header&)f[it->second];
						int filesize = parseoct(tarh.size, 12);
						tar.Open(it->first, fsWrite);
						tar.Write(f + ((it->second + sizeof(tar_header) + 512 - 1) & ~(512 - 1)), filesize);
						tar.Close();
					}
				}	
				break;
			}
		}
		file.Close();
		return true;
	}
}
/* Parse an octal number, ignoring leading and trailing nonsense. */
int PUP::parseoct(const char *p, size_t n)
{
	int i = 0;

	while ((*p < '0' || *p > '7') && n > 0) {
		++p;
		--n;
	}
	while (*p >= '0' && *p <= '7' && n > 0) {
		i *= 8;
		i += *p - '0';
		++p;
		--n;
	}
	return (i);
}
