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
    //BVHAccel *bvh;

    QVector<Object* > objects;
    QVector<QOpenGLShaderProgram*> shaderPrograms;
    QVector<PointLight*> pointlights;
    DirLight* dirlight;

    void Add(Object *object);
    void Add(PointLight* pointlight);
    void Add(QOpenGLShaderProgram* shaderprogram);

public:
    Scene();
};

#endif // SCENE_H
