#pragma once
#define MINIMAL_EMU_LOG

#include <QtWidgets/QMainWindow>
#include <QtWidgets>
#include "ui_pcsx3gui.h"
#include "GameListViewer.h"
#include "../pcsx3/src/emu.log.h"
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

void __forceinline vfstderrf(wchar_t const fmt[], va_list args) { wchar_t tmp[4096];  vswprintf(tmp, fmt, args);  qDebug() << tmp; }
void __forceinline vfstderrf(char    const fmt[], va_list args) { char tmp[4096];  vsprintf(tmp, fmt, args);  qDebug() << tmp; }
void __forceinline fstderrf(wchar_t const fmt[], ...) { va_list args; va_start(args, fmt); vfstderrf(fmt, args); va_end(args); }
void __forceinline fstderrf(char    const fmt[], ...) { va_list args; va_start(args, fmt); vfstderrf(fmt, args); va_end(args); }
