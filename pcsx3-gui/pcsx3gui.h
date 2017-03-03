#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_pcsx3gui.h"

class pcsx3gui : public QMainWindow
{
	Q_OBJECT

public:
	pcsx3gui(QWidget *parent = Q_NULLPTR);

private:
	Ui::pcsx3guiClass ui;
};
