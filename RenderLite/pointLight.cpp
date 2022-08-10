#include "pointLight.h"

float PointLight::ambient = pointLightDefaultAmbient;
float PointLight::diffuse = PointLightDefaultDiffuse;
float PointLight::specular = PointLightDefaultSpecular;
float PointLight::constant = PointLightDefaultConstant;
float PointLight::linear = PointLightDefaultLinear;
float PointLight::quadratic=PointLightDefaultQuadratic;

QMatrix4x4 PointLight::getLightMatrix()
{
    QMatrix4x4 lightProjection, lightView;

    float near_plane = 0.50f, far_plane = 100.5f;
    lightProjection.perspective(150.0f,1,near_plane,far_plane);
    //lightProjection.ortho(-eyeing, eyeing, -eyeing, eyeing, near_plane, far_plane);
    lightView.lookAt(position, position+lightNormal, QVector3D(0.0, 1.0, 0.0));
    lightSpaceMatrix = lightProjection * lightView;
    return lightSpaceMatrix;
}

void PointLight::setPosition(const QVector3D &newPosition)
{
    position = newPosition;
}

QVector3D &PointLight::getPosition()
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

QVector3D PointLight::getColor()
{
    return color*intensity;
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



