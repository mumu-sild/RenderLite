#ifndef GLOBAL_H
#define GLOBAL_H

#include <QString>
#include <QApplication>
#include <QDir>
#include <QDebug>

QString path;


void initGlobal(){

    QString fileName = QCoreApplication::applicationDirPath();
    QDir dir( fileName );
    dir.cdUp();
    dir.cdUp();
    path = dir.absolutePath()+"/RenderLite";
    qDebug()<<"path="<<path;
}


#endif // GLOBAL_H
