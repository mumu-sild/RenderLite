#ifndef SCENE_H
#define SCENE_H

#include <QVector3D>
#include <QVector>
#include <QOpenGLShaderProgram>
#include "Object.h"
#include "dirLight.h"
#include "pointLight.h"

class Scene
{
public:
    QVector3D backgroundColor = QVector3D(0.235294, 0.67451, 0.843137);
    //BVHAccel *bvh;

    QVector<Object* > objects;
    QVector<QOpenGLShaderProgram*> shaderProgram;
    QVector<PointLight*> pointlight;
    DirLight* light;

    void Add(Object *object);

public:
    Scene();
};

#endif // SCENE_H
