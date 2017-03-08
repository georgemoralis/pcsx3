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
	pkgSize = file.getFileSize();
	PKGHeader pkgheader;
	file.Read(&pkgheader, sizeof(pkgheader));

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
