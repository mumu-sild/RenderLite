#ifndef POINTLIGHT_H
#define POINTLIGHT_H
#include <QVector3D>
#include <QOpenGLShaderProgram>
#include <QOpenGLFramebufferObject>

#include "Setting.h"

class PointLight
{
public:
    static float ambient;
    static float diffuse;
    static float specular;

    static float constant;
    static float linear;
    static float quadratic;

    QVector3D lightNormal;
    QVector3D color;
    float width;
//    bool active;
    QVector3D position;
    QOpenGLFramebufferObject* depthMapFBO;
    QMatrix4x4 lightSpaceMatrix;




public:
    PointLight(QVector3D posi,QVector3D c){
        position = posi;
        color = c;
        depthMapFBO = new QOpenGLFramebufferObject(SHADOW_WIDTH,SHADOW_HEIGHT,QOpenGLFramebufferObject::Depth);

    }

    void setPosition(const QVector3D &newPosition);
    QVector3D &getPosition();
    void setPositionX(const float x);
    void setPositionY(const float y);
    void setPositionZ(const float z);


    QVector3D &getColor();
    void setColorR(const float r);
    void setColorG(const float g);
    void setColorB(const float b);

    QMatrix4x4 getLightMatrix();

};



#endif // POINTLIGHT_H
