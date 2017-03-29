#include "PKG.h"
#include "common.h"
#include "fsFile.h"
#include "sha1.h"
#include "aes.h"
#include <direct.h>  
#include <stdlib.h>  
#include <stdio.h> 
PKG::PKG()
{
}


PKG::~PKG()
{
}

bool PKG::open(const std::string& filepath) {
	fsFile file;
	if (!file.Open(filepath, fsRead))
	{
		return false;
	}
	pkgSize = file.getFileSize();
	PKGHeader pkgheader;
	file.Read(&pkgheader, sizeof(pkgheader));
	//todo check if magic is valid

	file.Seek(55, fsSeekSet);
	file.Read(&pkgTitleID, sizeof(pkgTitleID));

	//find the sha-1 in the last 32bytes of pkg
	file.Seek(pkgSize - 0x20, fsSeekSet);
	PKGFooter footer;
	file.Read(&footer, sizeof(footer));
	memcpy(pkgSHA1, footer.sha1, 20);
	file.Close();

	return true;
}
U64 PKG::getPkgSize()
{
	return pkgSize;
}
S08* PKG::getPkgSHA1()
{
	return pkgSHA1;
}
std::string PKG::getTitleID()
{
	return std::string(pkgTitleID);
}
bool PKG::extract(const std::string& filepath, const std::string& extractPath, std::string& failreason)
{
	this->extractPath = extractPath;
	fsFile file;
	if (!file.Open(filepath, fsRead))
	{
		return false;
	}
	pkgSize = file.getFileSize();
	PKGHeader pkgheader;
	file.Read(&pkgheader, sizeof(pkgheader));
	if (FromBigEndian(pkgheader.pkg_revision) != PKG_REVISION_RELEASE)
	{
		failreason="Debug pkgs not supported (yet)";
		return false;//we support only release pkgs for now
	}
	if (FromBigEndian(pkgheader.pkg_type) != PKG_TYPE_PS3)
	{
		failreason = "Only PS3 pkgs supported (yet)";
		return false; //we support only PS3 pkgs for now
	}
	if (FromBigEndian(pkgheader.total_size) > pkgSize)
	{
		failreason = "PKG file size is different";
		return false;
	}
	if (FromBigEndian(pkgheader.data_size + pkgheader.data_offset) > FromBigEndian(pkgheader.total_size))
	{
		failreason = "Data size is bigger than pkg size";
		return false;
	}
	//test code it will rewrite from here
	file.Seek(0, fsSeekSet);
	pkg = (U08 *)mmap(pkgSize,file.fileDescr());
	
	file.Read(pkg, pkgSize);
	offset = FromBigEndian(pkgheader.data_offset);
	U64 size = FromBigEndian(pkgheader.data_size);
	U08 iv[0x10];
	memcpy(iv, pkg + 0x70, 0x10);

	aes_context aes;
	U08  PS3AesKey[] = {
		0x2E, 0x7B, 0x71, 0xD7, 0xC9, 0xC9, 0xA1, 0x4E,
		0xA3, 0x22, 0x1F, 0x18, 0x88, 0x28, 0xB8, 0xF8
	};
	aes_setkey_enc(&aes, PS3AesKey, 128);

	U08 ctr_stream_block[0x10];
	U64 ctr_nc_off = 0;
	//aes_crypt_ctr(&aes, size, &ctr_nc_off, iv, ctr_stream_block, pkg+offset, pkg+offset);
	AES_CTR_encrypt(&aes, size, iv, pkg + offset, pkg + offset);
	
	char fname[256];
	n_files = FromBigEndian(pkgheader.item_count);
	
	for (int i = 0; i < n_files; i++) {
		PKGEntry entry = (PKGEntry&)pkg[offset + i * 0x20];
		U32 fname_len = FromBigEndian(entry.name_size);
		U32 fname_off = FromBigEndian(entry.name_offset);
		U32 flags = FromBigEndian(entry.type);
		U64 file_offset = FromBigEndian(entry.data_offset);
		U64 file_size = FromBigEndian(entry.data_size);

		memset(fname, 0, sizeof(fname));
		strncpy(fname, (char *)(pkg + offset + fname_off), fname_len);

		flags &= 0xff;
		if (flags == 4)
		{
			std::string path = extractPath + std::string(fname);
			_mkdir(path.c_str());//nasty but we will redo this
		}
		else
		{
			fsFile out;
			out.Open(extractPath + fname, fsWrite);
			out.Write(pkg + offset + file_offset, file_size);
			out.Close();
		}
	}
	munmap(pkg);
	return true;
}
U32 PKG::getNumberOfFiles()
{
	return n_files;
}

