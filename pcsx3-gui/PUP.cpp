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
				tar.Open("update_files.tar", fsWrite);
				char *f = new char[length];
				file.Seek(offset, fsSeekSet);
				file.Read(f, length);
				tar.Write(f, length);
				tar.Close();
				break;
			}
		}
		file.Close();
		return true;
	}
}

