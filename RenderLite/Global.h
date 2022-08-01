#pragma once

#include <QString>
#include <QApplication>
#include <QDir>
#include <QDebug>
#include <random>
#include <math.h>
#include "vec.h"

//----------------------------------------------------------------------
//                                常量区
//----------------------------------------------------------------------
#undef M_PI
#define M_PI 3.141592653589793f

const float kInfinity = std::numeric_limits<float>::max();
const float EPSILON = 0.00001;


//----------------------------------------------------------------------
//                             本地路径初始化
//----------------------------------------------------------------------

//*注：全局头文件不能使用全局函数，否则会被多次定义
//全局函数需要写在main.cpp

extern QString path;
inline void initGlobal(){

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

#define pi 3.1415926535

inline double min(double a,double b){
    return a<b?a:b;
}

inline double max(double a,double b){
    return a>b?a:b;
}

inline float degree_to_radians(float degree) {
    return degree * pi / 180.0;
}

//----------------------------------------------------------------------
//                                  random
//----------------------------------------------------------------------

inline double random_double() {
    static std::mt19937 generator;
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);

    return distribution(generator);
}

inline double random_double(double min, double max) {
    return min + random_double() * (max - min);
}

inline vec3 random() {
    return vec3(random_double(), random_double(), random_double());
}

inline vec3 random(double min, double max) {
    return vec3(random_double(min, max), random_double(min, max), random_double(min, max));
}

inline vec3 random_in_unit_sphere() {
    while (true) {
        auto p = random(-1, 1);
        if (p.length_squared() >= 1) continue;
        return p;
    }
}

inline vec3 random_unit_vector() {
    return normalize(random_in_unit_sphere());
}

inline vec3 random_in_unit_disk() {
    while (true) {
        auto p = vec3(random_double(-1, 1), random_double(-1, 1), 0);
        if (p.length_squared() >= 1) continue;
        return p;
    }
}
