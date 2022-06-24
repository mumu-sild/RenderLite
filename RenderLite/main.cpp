#include "mainwindow.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QDir>
#include "Global.h"
#include "include/assimp/Importer.hpp"


QString path;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    initGlobal();

    Assimp::Importer importer;

    MainWindow w;
    w.resize(w.sizeHint());//sizeHint���Ա����˲����Ľ����С
    int desktopArea = QApplication::desktop()->width() *
                     QApplication::desktop()->height();
    int widgetArea = w.width() * w.height();
    if (((float)widgetArea / (float)desktopArea) < 0.75f)
        w.show();
    else
        w.showMaximized();
    w.show();
    return a.exec();
}
