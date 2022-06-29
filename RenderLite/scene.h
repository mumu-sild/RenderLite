#ifndef SCENE_H
#define SCENE_H

#include <QVector3D>
#include <QVector>
#include <QOpenGLShaderProgram>
#include "Object.h"

class Scene
{
public:
    QVector3D backgroundColor = QVector3D(0.235294, 0.67451, 0.843137);
    //BVHAccel *bvh;

    QVector<Object* > objects;
    QVector<QOpenGLShaderProgram*> shaderProgram;

    void Add(Object *object);

public:
    Scene();
};

#endif // SCENE_H
