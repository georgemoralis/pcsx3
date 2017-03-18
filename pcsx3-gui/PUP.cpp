#include "types.h"
#include "PUP.h"
#include "fsFile.h"
#include "aes.h"
#include "..\pcsx3\ext\zlib\zlib.h"
#include <direct.h>  
#include <stdlib.h>  
#include <stdio.h> 
#include "../pcsx3-gui/pcsx3gui.h"

using namespace std;

PUP::PUP()
{

}
PUP::~PUP()
{

}
bool PUP::Read(const std::string& filepath, const std::string& extractPath)
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
		printPUPHeader(pupheader);
		
		U64 filenumber = FromBigEndian(pupheader.file_count);
		for (int i = 0; i < FromBigEndian(pupheader.file_count); i++)
		{
			file.Seek(0x30 + (0x20 * i), fsSeekSet);
			PUPFileEntry fentry;
			file.Read(&fentry, sizeof(fentry));
			if (FromBigEndian(fentry.entry_id) == 0x300)
			{
				printPUPFileEntries(pupheader, fentry);
				U64 length = FromBigEndian(fentry.data_length);
				U64 offset = FromBigEndian(fentry.data_offset);
	
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
					if (it->first.find("dev_flash_") == 0)//find and extract dev_flash files for now
					{
						tar_header& tarh = (tar_header&)f[it->second];
						int filesize = parseoct(tarh.size, 12);
						int foffset = ((it->second + sizeof(tar_header) + 512 - 1) & ~(512 - 1));
						//store file in temp memory
						U08 *devf = new U08[filesize];
						memcpy(devf, f + foffset, filesize);

						//decrypt pkg
						U32 decryptedfilesize = 0;
						U08* decrypted =decryptpkg(devf, decryptedfilesize);
						int off = 0;
						for (;;)
						{
							tar_header& tarh = (tar_header&)decrypted[off];
							int filesize = parseoct(tarh.size, 12);
							if (tarh.typeflag == '5')
							{
								_mkdir(tarh.name);//create_dir((char*)decrypted, /*parseoct(decrypted + 100, 8)*/0);
							}
							if (tarh.typeflag == '0')
							{
								int foffset = ((off + sizeof(tar_header) + 512 - 1) & ~(512 - 1));
								fsFile tar;
								tar.Open(tarh.name, fsWrite);
								tar.Write(foffset+decrypted, filesize);
								tar.Close();
							}
							off += ((filesize + sizeof(tar_header) + 512 - 1) & ~(512 - 1));//alligned to 512 bytes
							if (off > decryptedfilesize)
								break;
						}

						delete[] devf;
						delete[] decrypted;
					}
				}
				delete[] f;
			}
			else //just print entry info
			{
				printPUPFileEntries(pupheader, fentry);
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
/*
    decypt pkg from pup . This is a SCE file so it would be generic for self's as well later
*/
U08* PUP::decryptpkg(U08 *pkg,U32 &filesize)
{
	U16 flags;
	U16 type;
	U32 hdr_len;
	U64 dec_size;
	struct keylist *k;
	const auto& sce_header = (SceHeader&)pkg[0x0];

	flags = FromBigEndian(sce_header.flags);
	type = FromBigEndian(sce_header.type);
	//hdr_len = FromBigEndian((U32&)pkg + 0x10);
	//dec_size = FromBigEndian((U64&)pkg + 0x18);
	sce_decrypt_header(pkg);
	filesize = FromBigEndian(sce_header.esize)-0x80;
	U08* extracted = new U08[filesize];
	sce_decrypt_data(pkg,extracted);
	return extracted;
}
void PUP::sce_decrypt_header(U08 *ptr)
{
	U32 meta_offset;
	U32 meta_len;
	U64 header_len;
	U32 i, j;
	U08 tmp[0x40];
	int success = 0;

	U08 SCEPKG_RIV[0x10] = {
		0x4A, 0xCE, 0xF0, 0x12, 0x24, 0xFB, 0xEE, 0xDF, 0x82, 0x45, 0xF8, 0xFF, 0x10, 0x21, 0x1E, 0x6E
	};

	U08 SCEPKG_ERK[0x20] = {
		0xA9, 0x78, 0x18, 0xBD, 0x19, 0x3A, 0x67, 0xA1, 0x6F, 0xE8, 0x3A, 0x85, 0x5E, 0x1B, 0xE9, 0xFB, 0x56, 0x40, 0x93, 0x8D,
		0x4D, 0xBC, 0xB2, 0xCB, 0x52, 0xC5, 0xA2, 0xF8, 0xB0, 0x2B, 0x10, 0x31
	};

	const auto& sce_header = (SceHeader&)ptr[0x0];
	meta_offset = FromBigEndian(sce_header.meta);
	header_len = FromBigEndian(sce_header.hsize);

	aes_context aes;
	// Decrypt Metadata Info
	U08 metadata_iv[0x10];
	memcpy(metadata_iv, SCEPKG_RIV, 0x10);
	aes_setkey_dec(&aes, SCEPKG_ERK, 256);
	auto& meta_info = (MetadataInfo&)ptr[sizeof(SceHeader) + FromBigEndian(sce_header.meta)];

	aes_crypt_cbc(&aes, AES_DECRYPT, sizeof(MetadataInfo), metadata_iv, (U08*)&meta_info, (U08*)&meta_info);

	// Decrypt Metadata Headers (Metadata Header + Metadata Section Headers)
	U08 ctr_stream_block[0x10];
	U08* meta_headers = (U08*)&ptr[sizeof(SceHeader) + FromBigEndian(sce_header.meta) + sizeof(MetadataInfo)];
	U32 meta_headers_size = FromBigEndian(sce_header.hsize) - (sizeof(SceHeader) + FromBigEndian(sce_header.meta) + sizeof(MetadataInfo));
	U64 ctr_nc_off = 0;
	aes_setkey_enc(&aes, meta_info.key, 128);
	aes_crypt_ctr(&aes, meta_headers_size, &ctr_nc_off, meta_info.iv, ctr_stream_block, meta_headers, meta_headers);

}
void PUP::sce_decrypt_data(U08 *ptr,U08 *extracted)
{
	const auto& sce_header = (SceHeader&)ptr[0x0];
	const U32 meta_header_off = sizeof(SceHeader) + FromBigEndian(sce_header.meta) + sizeof(MetadataInfo);
	const auto& meta_header = (MetadataHeader&)ptr[meta_header_off];
	const U08* data_keys = (U08*)&ptr[meta_header_off + sizeof(MetadataHeader) + FromBigEndian(meta_header.section_count) * sizeof(MetadataSectionHeader)];
	aes_context aes;
	
	U08* data = new U08[FromBigEndian(sce_header.esize)];
	for (U32 i = 0; i < FromBigEndian(meta_header.section_count); i++) {
		const auto& meta_shdr = (MetadataSectionHeader&)ptr[meta_header_off + sizeof(MetadataHeader) + i * sizeof(MetadataSectionHeader)];
		U08* data_decrypted = new U08[FromBigEndian(meta_shdr.data_size)];

		U08 data_key[0x10];
		U08 data_iv[0x10];
		if (FromBigEndian(meta_shdr.key_idx) == 0xffffffff || FromBigEndian(meta_shdr.iv_idx) == 0xffffffff)// first shdr appears to have some kind of info that doesn't seem to be encrpyted (in PUP files)
			continue;
		if (FromBigEndian(meta_shdr.encrypted) == 3)
		{
			memcpy(data_key, data_keys + FromBigEndian(meta_shdr.key_idx) * 0x10, 0x10);
			memcpy(data_iv, data_keys + FromBigEndian(meta_shdr.iv_idx) * 0x10, 0x10);
			memcpy(data_decrypted, &ptr[FromBigEndian(meta_shdr.data_offset)], FromBigEndian(meta_shdr.data_size));

			// Perform AES-CTR encryption on the data
			U08 ctr_stream_block[0x10] = {};
			U64 ctr_nc_off = 0;
			aes_setkey_enc(&aes, data_key, 128);
			aes_crypt_ctr(&aes, FromBigEndian(meta_shdr.data_size), &ctr_nc_off, data_iv, ctr_stream_block, data_decrypted, data_decrypted);
		}
		if (FromBigEndian(meta_shdr.compressed) == 2)
		{
			unsigned long length = FromBigEndian(sce_header.esize) - 0x80;
			uncompress(data, &length, data_decrypted, (U32)FromBigEndian(meta_shdr.data_size));
			memcpy(extracted, data, length);
		}
		
		delete[] data_decrypted;
		delete[] data;
	}

}

//debug info
void PUP::printPUPHeader(PUPHeader puph)
{
	infof(PUP, "%-30s 0x%x","magic : ",FromBigEndian(puph.magic)>>32);
	infof(PUP, "%-30s 0x%x","package_version : ",FromBigEndian(puph.package_version));
	infof(PUP, "%-30s 0x%x","image_version : ", FromBigEndian(puph.image_version));
	infof(PUP, "%-30s 0x%x","file_count : ", FromBigEndian(puph.file_count));
	infof(PUP, "%-30s 0x%x","header_length : ", FromBigEndian(puph.header_length));
	infof(PUP, "%-30s 0x%x","data_length : ", FromBigEndian(puph.data_length));
}
void PUP::printPUPFileEntries(PUPHeader puph, PUPFileEntry fentry)
{
	infof(PUP, "%-30s %s", "entry_name : ", id2name(FromBigEndian(fentry.entry_id), entries,NULL));
	infof(PUP, "%-30s 0x%x", "entry_id : ", FromBigEndian(fentry.entry_id));
	infof(PUP, "%-30s 0x%x", "data_offset : ", FromBigEndian(fentry.data_offset));
	infof(PUP, "%-30s 0x%x", "data_length : ", FromBigEndian(fentry.data_length));
}