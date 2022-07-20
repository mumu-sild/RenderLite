#ifndef POINTLIGHT_H
#define POINTLIGHT_H
#include <QVector3D>
#include <QOpenGLShaderProgram>

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
//    bool active;
    QVector3D position;


public:
    PointLight(QVector3D posi,QVector3D c){
        position = posi;
        color = c;
    }

//    void setPointLight(QVector<PointLight*>& pointLight, QOpenGLShaderProgram* shader);
    void setPosition(const QVector3D &newPosition);
    QVector3D &getPosition();
    void setPositionX(const float x);
    void setPositionY(const float y);
    void setPositionZ(const float z);


    QVector3D &getColor();
    void setColorR(const float r);
    void setColorG(const float g);
    void setColorB(const float b);

};



#endif // POINTLIGHT_H
