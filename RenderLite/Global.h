#ifndef GLOBAL_H
#define GLOBAL_H

#include <QString>
#include <QApplication>
#include <QDir>
#include <QDebug>

//----------------------------------------------------------------------
//                                ������
//----------------------------------------------------------------------
#undef M_PI
#define M_PI 3.141592653589793f

const float kInfinity = std::numeric_limits<float>::max();



//----------------------------------------------------------------------
//                             ����·����ʼ��
//----------------------------------------------------------------------

QString path;
void initGlobal(){

    QString fileName = QCoreApplication::applicationDirPath();
    QDir dir( fileName );
    dir.cdUp();
    dir.cdUp();
    path = dir.absolutePath()+"/RenderLite";
    qDebug()<<"path="<<path;
}

//----------------------------------------------------------------------
//                                  math
//----------------------------------------------------------------------

inline double min(double a,double b){
    return a<b?a:b;
}

inline double max(double a,double b){
    return a>b?a:b;
}


#endif // GLOBAL_H
