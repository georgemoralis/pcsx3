#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_pcsx3gui.h"
#include "GameListViewer.h"
class pcsx3gui : public QMainWindow
{
	Q_OBJECT

public:
	pcsx3gui(QWidget *parent = Q_NULLPTR);

private:
	Ui::pcsx3guiClass ui;
	GameListViewer* game_list;

public slots:
    void installPKG();
	void installFW();
};
