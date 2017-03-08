#include <QDir>
#include <QFileDialog>
#include <QProgressDialog>
#include <QMessageBox>
#include "pcsx3gui.h"
#include "pkg.h"
#include "sha1.h"
#include "fsFile.h"

pcsx3gui::pcsx3gui(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	game_list = new GameListViewer();
	game_list->SetGamePath(QDir::currentPath() + "/dev_hdd0/game/");
	ui.horizontalLayout->addWidget(game_list);
	show();
	game_list->PopulateAsync();
}
void pcsx3gui::installPKG()
{
	std::string file(QFileDialog::getOpenFileName(this, tr("Install PKG File"), QDir::currentPath(), tr("PKG File (*.PKG)")).toStdString());

	PKG pkg;
	pkg.open(file);
	int numfiles = pkg.getPkgSize() /8192;
	QProgressDialog progress("Verifying PKG", "Cancel", 0, numfiles, this);
	unsigned char buf[8192];
	sha1_context ctx;
	unsigned char resultsha1[20];
	sha1_starts(&ctx);

	progress.setWindowModality(Qt::WindowModal);
	progress.setCancelButton(NULL);
	progress.setValue(0);

	U64 remainingsize = pkg.getPkgSize();
	int count = 0;
	fsFile fsfile;
	fsfile.Open(file, fsRead);
	while (remainingsize > 8192)
	{
		size_t len;
		len = fsfile.ReadBytes(buf, sizeof(buf));
		sha1_update(&ctx, buf, len);
		remainingsize -= len;
		progress.setValue(++count);
	}
	if (remainingsize)//last block remove the last 32 bytes which are the sha1 footer of pkg
	{
		size_t len;
		len = fsfile.ReadBytes(buf, sizeof(buf));
		sha1_update(&ctx, buf, len - 0x20);
	}
	
	progress.setValue(numfiles);
	sha1_finish(&ctx, resultsha1);
	memset(&ctx, 0, sizeof(sha1_context));
	fsfile.Close();//close file used for calculate sha1
	if (memcmp(pkg.getPkgSHA1(), resultsha1, 20) != 0)
	{
		QMessageBox::critical(this, "Verification Failed!", "Package's SHA-1 and calculated SHA-1 are not the same", QMessageBox::Ok, 0);
	}
	else
	{
		QMessageBox::information(this, "Verification OK!", "Package's SHA-1 is correct", QMessageBox::Ok, 0);
	}
}
