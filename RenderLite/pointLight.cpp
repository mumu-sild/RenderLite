#include "pointLight.h"
#include "lightData.h"

//void PointLight::setPointLight(QVector<PointLight*>& pointLight, QOpenGLShaderProgram *shader)
//{
//    LightData* lightData = new LightData(shader);


//    lightData->activatePointLight();
//    lightData->setPointLightPosition(pointLight);
//    lightData->setPointLightAmbientColor(pointLight,PointLight::ambient);
//    lightData->setPointLightDiffuseColor(pointLight,PointLight::diffuse);
//    lightData->setPointLightSpecularColor(pointLight,PointLight::specular);
//    lightData->setConstant(constant);
//    lightData->setLinear(linear);
//    lightData->setQuadratic(quadratic);
//    delete lightData;
//    return;
//}

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

QVector3D &PointLight::getColor()
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



