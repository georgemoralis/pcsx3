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
	file.ReadBE(pkgheader);
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
	file.ReadBE(pkgheader);
	if (pkgheader.pkg_revision != PKG_REVISION_RELEASE)
	{
		failreason="Debug pkgs not supported (yet)";
		return false;//we support only release pkgs for now
	}
	if (pkgheader.pkg_type != PKG_TYPE_PS3)
	{
		failreason = "Only PS3 pkgs supported (yet)";
		return false; //we support only PS3 pkgs for now
	}
	if (pkgheader.total_size > pkgSize)
	{
		failreason = "PKG file size is different";
		return false;
	}
	if ((pkgheader.data_size + pkgheader.data_offset) > pkgheader.total_size)
	{
		failreason = "Data size is bigger than pkg size";
		return false;
	}
	//test code it will rewrite from here
	file.Seek(0, fsSeekSet);
	pkg = (U08 *)mmap(pkgSize,file.fileDescr());
	
	file.Read(pkg, pkgSize);
	offset = pkgheader.data_offset;
	U64 size = pkgheader.data_size;
	U08 iv[0x10];
	memcpy(iv, pkgheader.klicensee, 0x10);

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
	n_files = pkgheader.item_count;
	
	for (int i = 0; i < n_files; i++) {
		PKGEntry entry = (PKGEntry&)pkg[offset + i * 0x20];
		ReadBE(entry);

		memset(fname, 0, sizeof(fname));
		strncpy(fname, (char *)(pkg + offset + entry.name_offset), entry.name_size);

		entry.type &= 0xff;
		if (entry.type == 4)
		{
			std::string path = extractPath + std::string(fname);
			_mkdir(path.c_str());//nasty but we will redo this
		}
		else
		{
			fsFile out;
			out.Open(extractPath + fname, fsWrite);
			out.Write(pkg + offset + entry.data_offset, entry.data_size);
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

