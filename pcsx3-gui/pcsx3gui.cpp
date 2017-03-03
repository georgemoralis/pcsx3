#include <QDir>
#include "pcsx3gui.h"

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
