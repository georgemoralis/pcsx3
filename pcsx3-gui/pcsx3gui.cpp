#include <QDir>
#include <QFileDialog>
#include <QtConcurrent>
#include <QProgressDialog>
#include <qtconcurrentmap.h>
#include <QMessageBox>
#include "pcsx3gui.h"
#include "pkg.h"
#include "sha1.h"
#include "fsFile.h"
#include "loader.h"
#include "PUP.h"
#include <functional>

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
	if (detectFileType(file) == FILETYPE_PKG)
	{
		PKG pkg;
		pkg.open(file);
		int numfiles = pkg.getPkgSize() / 8192;
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
			

			//if pkg is ok we procced with extraction
			std::string failreason;
			QString gamedir = QDir::currentPath() + "/dev_hdd0/game/" + QString::fromStdString(pkg.getTitleID());
			QDir dir(gamedir);
			if (!dir.exists()) {
				dir.mkpath(".");
			}
			std::string extractpath = QDir::currentPath().toStdString() + "/dev_hdd0/game/" + pkg.getTitleID() + "/";
			if (!pkg.extract(file, extractpath, failreason))
			{
				QMessageBox::critical(this, "PKG ERROR", QString::fromStdString(failreason), QMessageBox::Ok, 0);
			}
			else
			{
				int nfiles = pkg.getNumberOfFiles();
				QVector<int> vector;
				for (int i = 0; i < nfiles; ++i)
					vector.append(i);

				// Create a progress dialog.
				QProgressDialog dialog;
				dialog.setLabelText("Extracting PKG please wait");

				// Create a QFutureWatcher and connect signals and slots.
				QFutureWatcher<void> futureWatcher;
				QObject::connect(&futureWatcher, SIGNAL(finished()), &dialog, SLOT(reset()));
				QObject::connect(&dialog, SIGNAL(canceled()), &futureWatcher, SLOT(cancel()));
				QObject::connect(&futureWatcher, SIGNAL(progressRangeChanged(int, int)), &dialog, SLOT(setRange(int, int)));
				QObject::connect(&futureWatcher, SIGNAL(progressValueChanged(int)), &dialog, SLOT(setValue(int)));

				// Start the computation.
				futureWatcher.setFuture(QtConcurrent::map(vector, std::bind(&PKG::extractfiles, pkg, std::placeholders::_1)));

				// Display the dialog and start the event loop.
				dialog.exec();

				futureWatcher.waitForFinished();
				pkg.clearBuffer();
				QMessageBox::information(this, "Extraction Finished", "Game successfully installed at " + gamedir, QMessageBox::Ok, 0);
				game_list->RefreshGameDirectory();//force refreshing since filelistwatcher doesn't work properly
			}
		}
	}
	else
	{
		QMessageBox::critical(this, "PKG ERROR", "File doesn't appear to be a valid PKG file", QMessageBox::Ok, 0);
	}
}
void pcsx3gui::installFW()
{
	std::string file(QFileDialog::getOpenFileName(this, tr("Open FW Update Files"), QDir::currentPath(), tr("PUP File (*.PUP)")).toStdString());
	if (detectFileType(file) == FILETYPE_PUP)
	{
		std::string extractpath = QDir::currentPath().toStdString() + "/dev_flash/";
		QDir dir(QString::fromStdString(extractpath));
		if (!dir.exists()) {
			dir.mkpath(".");
		}
		PUP pup;
		pup.Read(file,extractpath);
		QMessageBox::information(this, "Extraction Finished", "FW installation completed", QMessageBox::Ok, 0);
	}
	else
	{
		QMessageBox::critical(this, "PUP ERROR", "File doesn't appear to be a valid PUP file", QMessageBox::Ok, 0);
	}
}