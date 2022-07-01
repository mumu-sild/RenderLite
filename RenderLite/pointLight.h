#ifndef POINTLIGHT_H
#define POINTLIGHT_H
#include <QVector3D>

class PointLight
{
public:
    static float ambient;
    static float diffuse;
    static float specular;

    static float constant;
    static float linear;
    static float quadratic;


    const QVector3D &getPosition() const;
    void setPositionX(const float x);
    void setPositionY(const float y);
    void setPositionZ(const float z);



    const QVector3D &getColor() const;
    void setColorR(const float r);
    void setColorG(const float g);
    void setColorB(const float b);

private:
    QVector3D position;
    QVector3D color;
    bool pointLightActivated = false;


};

#endif // POINTLIGHT_H
