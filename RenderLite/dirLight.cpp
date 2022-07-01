#include "dirLight.h"


const QVector3D &DirLight::getDirection() const
{
    return direction;
}

void DirLight::setDirectionX(const float x)
{
    direction.setX(x);
}

void DirLight::setDirectionY(const float y)
{
    direction.setY(y);
}

void DirLight::setDirectionZ(const float z)
{
    direction.setZ(z);
}

const QVector3D &DirLight::getColor() const
{
    return color;
}

void DirLight::setColorR(const float r)
{
    color.setX(r);
}

void DirLight::setColorG(const float g)
{
    color.setY(g);
}

void DirLight::setColorB(const float b)
{
    color.setZ(b);
}
