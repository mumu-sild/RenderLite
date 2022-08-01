#ifndef DIRLIGHT_H
#define DIRLIGHT_H
#include <QVector3D>
#include <QOpenGLShaderProgram>
#include <QOpenGLFramebufferObject>

#include "Setting.h"

class DirLight
{
public:
    static float ambient;
    static float diffuse;
    static float specular;
private:
    QVector3D direction;
    QVector3D color;
    QMatrix4x4 lightSpaceMatrix;
public:
    bool Activated = true;
    QOpenGLFramebufferObject* depthMapFBO;


public:
    QVector3D &getDirection();
    void setDirectionX(const float x);
    void setDirectionY(const float y);
    void setDirectionZ(const float z);

    QVector3D &getColor();
    void setColorR(const float r);
    void setColorG(const float g);
    void setColorB(const float b);

    QMatrix4x4 getLightMatrix();

//    void setDirLight(DirLight* dirlight, QOpenGLShaderProgram* shader);
    DirLight(QVector3D dir = QVector3D(-0.2f,-1.0f,-0.3f),
             QVector3D color = QVector3D(1,1,1))
        :direction(dir),color(color)
    {
        depthMapFBO = new QOpenGLFramebufferObject(SHADOW_WIDTH,SHADOW_HEIGHT,QOpenGLFramebufferObject::Depth);
    }

    ~DirLight(){
        delete depthMapFBO;
    }

};

#endif // DIRLIGHT_H
