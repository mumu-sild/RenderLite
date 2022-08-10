#include "dirLight.h"

float DirLight::ambient = DirLightDefaultAmbient;
float DirLight::diffuse = DirLightDefaultDiffuse;
float DirLight::specular = DirLightDefaultSpecular;

QVector3D &DirLight::getDirection()
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

QVector3D &DirLight::getColor()
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

QMatrix4x4 DirLight::getLightMatrix()
{
    QVector3D lightPos = -direction.normalized()*50;
    QMatrix4x4 lightProjection, lightView;

    float near_plane = -50.50f, far_plane = 100.5f;
    const float eyeing = 100.0f;
    lightProjection.ortho(-eyeing, eyeing, -eyeing, eyeing, near_plane, far_plane);
    lightView.lookAt(lightPos, QVector3D(0,0,0), QVector3D(0.0, 1.0, 0.0));
    lightSpaceMatrix = lightProjection * lightView;
    return lightSpaceMatrix;
}
