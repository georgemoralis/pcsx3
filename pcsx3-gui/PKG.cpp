#include "PKG.h"
#include "common.h"
#include "fsFile.h"
#include "sha1.h"

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
	const U64 pkgSize = file.getFileSize();
	PKGHeader pkgheader;
	file.Read(&pkgheader, sizeof(pkgheader));
	//todo read the rest of data

	file.Seek(pkgSize - 0x20, fsSeekSet);
	PKGFooter footer;
	file.Read(&footer, sizeof(footer));
	file.Close();

	unsigned char buf[8192];
	sha1_context ctx;
	unsigned char resultsha1[20];
	sha1_starts(&ctx);
	file.Open(filepath, fsRead);
	U64 remainingsize=pkgSize;
	while (remainingsize > 8192)
	{
		size_t len;
		len = file.ReadBytes(buf, sizeof(buf));
		sha1_update(&ctx, buf, len);
		remainingsize -= len;
	}
	if (remainingsize)//last block remove the last 32 bytes which are the sha1 footer of pkg
	{
		size_t len;
		len = file.ReadBytes(buf, sizeof(buf));
		sha1_update(&ctx, buf, len-0x20);
	}
	sha1_finish(&ctx, resultsha1);
	memset(&ctx, 0, sizeof(sha1_context));

	if (memcmp(footer.sha1, resultsha1, 20) != 0)
	{
		return false;
	}
	return true;
}
