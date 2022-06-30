#include "glwidget.h"

void GLWidget::setDirLight(bool activate, int objNum)
{
    lightData = new LightData(shaderSelector.getShader(objNum));
    if(!activate){
        lightData->disableDirLight();
        return;
    }
        lightData->activateDirLight();
        lightData->setDirLightDirection(dirLightDirection);
        lightData->setDirLightAmbientColor(dirLightColor,dirAmbient);
        lightData->setDirLightDiffuseColor(dirLightColor,dirDiffuse);
        lightData->setDirLightSpecularColor(dirLightColor,dirSpecular);
        delete lightData;
        return;
}

void GLWidget::setPointLight(bool activate, int objNum)
{
    lightData = new LightData(shaderSelector.getShader(objNum));
    if(!activate){
        lightData->disablepointLight();
        return;
    }
    lightData->activatePointLight();
    lightData->setPointLightPosition(pointLightPosition);
    lightData->setPointLightAmbientColor(pointLightColor,pointAmbient);
    lightData->setPointLightDiffuseColor(pointLightColor,pointDiffuse);
    lightData->setPointLightSpecularColor(pointLightColor,pointSpecular);
    lightData->setConstant(constant);
    lightData->setLinear(linear);
    lightData->setQuadratic(quadratic);
    delete lightData;
    return;
}
