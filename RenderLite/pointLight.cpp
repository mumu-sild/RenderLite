#include "pointLight.h"


const QVector3D &PointLight::getPosition() const
{
    return position;
}

void PointLight::setPositionX(const float x)
{
    position.setX(x);
}

void PointLight::setPositionY(const float y)
{
    position.setY(y);
}

void PointLight::setPositionZ(const float z)
{
    position.setZ(z);
}

const QVector3D &PointLight::getColor() const
{
    return color;
}

void PointLight::setColorR(const float r)
{
    color.setX(r);
}
void PointLight::setColorG(const float g)
{
    color.setX(g);
}
void PointLight::setColorB(const float b)
{
    color.setX(b);
}


