#include "dirLight.h"
#include "lightData.h"

//void DirLight::setDirLight(DirLight *dirlight, QOpenGLShaderProgram* shader)
//{
//    LightData* lightData = new LightData(shader);
//    if(!dirlight->dirLightActivated){
//        lightData->disableDirLight();
//        return;
//    }
//        lightData->activateDirLight();
//        lightData->setDirLightDirection(dirlight->getDirection());
//        lightData->setDirLightAmbientColor(dirlight->getColor(),DirLight::ambient);
//        lightData->setDirLightDiffuseColor(dirlight->getColor(),DirLight::diffuse);
//        lightData->setDirLightSpecularColor(dirlight->getColor(),DirLight::specular);
//        delete lightData;
//        return;
//}

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
