#include "shaderSelector.h"
#include "lightData.h"

ShaderSelector::ShaderSelector(){}

void ShaderSelector::setLightDir(int shader, DirLight *dirlight)
{
    LightData* lightData = new LightData(getShader(shader));
    if(!dirlight->dirLightActivated){
        lightData->disableDirLight();
        return;
    }
        lightData->activateDirLight();
        lightData->setDirLightDirection(dirlight->getDirection());
        lightData->setDirLightAmbientColor(dirlight->getColor(),DirLight::ambient);
        lightData->setDirLightDiffuseColor(dirlight->getColor(),DirLight::diffuse);
        lightData->setDirLightSpecularColor(dirlight->getColor(),DirLight::specular);
        delete lightData;
        return;
}

void ShaderSelector::setPointDir(int shader, QVector<PointLight *> pointlights)
{
    LightData* lightData = new LightData(getShader(shader));


    lightData->activatePointLight();
    lightData->setPointLightPosition(pointlights);
    lightData->setPointLightAmbientColor(pointlights,PointLight::ambient);
    lightData->setPointLightDiffuseColor(pointlights,PointLight::diffuse);
    lightData->setPointLightSpecularColor(pointlights,PointLight::specular);
    lightData->setPointLightNormal(pointlights);
    lightData->setConstant(PointLight::constant);
    lightData->setLinear(PointLight::linear);
    lightData->setQuadratic(PointLight::quadratic);
    lightData->setPointLightColor(pointlights);
    delete lightData;
    return;
}

void ShaderSelector::compileShader(int i){
    if(i < 0||i >= vertexPath.size()){
        qDebug()<<"shaderType out of bounds";
        return;
    }
    QOpenGLShaderProgram* shader = new QOpenGLShaderProgram();
    shader->addShaderFromSourceFile(QOpenGLShader::Vertex,vertexPath[i]);
    shader->addShaderFromSourceFile(QOpenGLShader::Fragment,fragmentPath[i]);
    shader->link();
    qDebug()<<"compile shader"<<i<<":"<<vertexPath[i];
    shaderProgram.push_back(shader);
}

QOpenGLShaderProgram* ShaderSelector::getShader(int objNum){
    if(shaderProgram.isEmpty()){
        qDebug()<<"shaderProgram is empty";
    }
    if(objNum < 0 || objNum >= shaderProgram.size()){
        qDebug()<<"haven't assign a shader to object";
    }
    return shaderProgram[objNum];
}
