#include <QDir>
#include <QFileDialog>
#include "pcsx3gui.h"
#include "pkg.h"

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
}
