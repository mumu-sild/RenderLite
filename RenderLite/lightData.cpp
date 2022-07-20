#include "lightData.h"

LightData::LightData(QOpenGLShaderProgram* shaderProgram):shader(shaderProgram){
};

void LightData::setDirLightDirection(QVector3D& direction){
    shader->setUniformValue("dirLight.direction",direction);
}

void LightData::setDirLightAmbientColor(QVector3D Color,float ambient){
    shader->setUniformValue("dirLight.ambient",Color * ambient);
}

void LightData::setDirLightDiffuseColor(QVector3D Color, float diffuse){
    shader->setUniformValue("dirLight.diffuse",Color * diffuse);
}
void LightData::setDirLightSpecularColor(QVector3D Color, float specular){
//    if(!dirLightActivated){
//        qDebug()<<"DirLight is not activated";
//        return;
//    }
    shader->setUniformValue("dirLight.specular",Color * specular);
}

void LightData::activatePointLight(QVector<PointLight *> &pointLight){
//    pointLightActivated = true;
    QString structNameFront = "pointLights[";
    QString structNameRail = "].pointLightActivated";
//    shader->setUniformValue("pointLightActivated",true);
    for(int i = 0; i < numPointLight; i++){
        QString StringI = structNameFront+i+structNameRail;
        //shader->setUniformValue((StringI).toStdString().c_str(),pointLight.at(i)->active);
    }
}

void LightData::disablepointLight(QVector<PointLight *> &pointLight)
{
//    pointLightActivated = false;
//    shader->setUniformValue("pointLightActivated",false);
    QString structNameFront = "pointLights[";
    QString structNameRail = "].pointLightActivated";
//    shader->setUniformValue("pointLightActivated",true);
    for(int i = 0; i < numPointLight; i++){
        QString StringI = structNameFront+i+structNameRail;
        //shader->setUniformValue((StringI).toStdString().c_str(),pointLight.at(i)->active);
    }
}

void LightData::setPointLightColor(QVector<PointLight *> &pointLight)
{
//    if(!pointLightActivated){
//        qDebug()<<"PointLight is not activated";
//        return;
//    }
    QString structNameFront = "pointLights[";
    QString structNameRail = "].color";
    for(int i = 0; i < numPointLight; i++){
        QString StringI = structNameFront+i+structNameRail;
        shader->setUniformValue((StringI).toStdString().c_str(),pointLight[i]->color);
    }
}

void LightData::setPointLightPosition(QVector<PointLight*>& pointLight){
//    if(!pointLightActivated){
//        qDebug()<<"PointLight is not activated";
//        return;
//    }
    numPointLight = pointLight.size();
    shader->setUniformValue("numPointLights",numPointLight);

    std::string structNameFront = "pointLights[";
    std::string structNameRail = "].position";
    //qDebug()<<"numPoints"<<numPointLight;
    for(int i = 0; i < numPointLight; i++){
        std::string StringI = std::to_string(i);
//        qDebug()<<(structNameFront+StringI+structNameRail).c_str();
        shader->setUniformValue((structNameFront+StringI+structNameRail).c_str(),pointLight[i]->getPosition());
    }
}

void LightData::setPointLightAmbientColor(QVector<PointLight*> pointLight,float ambient){
//    if(!pointLightActivated){
//        qDebug()<<"PointLight is not activated";
//        return;
//    }
    std::string structNameFront = "pointLights[";
    std::string structNameRail = "].ambient";
    for(int i = 0; i < numPointLight; i++){
        std::string StringI = std::to_string(i);
//        qDebug()<<(structNameFront+StringI+structNameRail).c_str();
        shader->setUniformValue((structNameFront+StringI+structNameRail).c_str(),pointLight[i]->getColor() * ambient);
    }
}

void LightData::setPointLightDiffuseColor(QVector<PointLight*> pointLight, float diffuse){
//    if(!pointLightActivated){
//        qDebug()<<"PointLight is not activated";
//        return;
//    }
    std::string structNameFront = "pointLights[";
    std::string structNameRail = "].diffuse";
    for(int i = 0; i < numPointLight; i++){
        std::string StringI = std::to_string(i);
//        qDebug()<<(structNameFront+StringI+structNameRail).c_str();
        shader->setUniformValue((structNameFront+StringI+structNameRail).c_str(),pointLight[i]->getColor() * diffuse);
    }
}

void LightData::setPointLightSpecularColor(QVector<PointLight*> pointLight, float specular){
//    if(!pointLightActivated){
//        qDebug()<<"PointLight is not activated";
//        return;
//    }
    std::string structNameFront = "pointLights[";
    std::string structNameRail = "].specular";
    for(int i = 0; i < numPointLight; i++){
        std::string StringI = std::to_string(i);
//        qDebug()<<(structNameFront+StringI+structNameRail).c_str();
        shader->setUniformValue((structNameFront+StringI+structNameRail).c_str(),pointLight[i]->getColor() * specular);
    }
}

void LightData::setPointLightNormal(QVector<PointLight *> pointLight)
{
//    if(!pointLightActivated){
//        qDebug()<<"PointLight is not activated";
//        return;
//    }
    std::string structNameFront = "pointLights[";
    std::string structNameRail = "].lightnormal";
    for(int i = 0; i < numPointLight; i++){
        std::string StringI = std::to_string(i);
//        qDebug()<<(structNameFront+StringI+structNameRail).c_str();
        shader->setUniformValue((structNameFront+StringI+structNameRail).c_str(),pointLight[i]->lightNormal);
    }
}

void LightData::setConstant(float constant){
//    if(!pointLightActivated){
//        qDebug()<<"PointLight is not activated";
//        return;
//    }
    std::string structNameFront = "pointLights[";
    std::string structNameRail = "].constant";
    for(int i = 0; i < numPointLight; i++){
        std::string StringI = std::to_string(i);
//        qDebug()<<(structNameFront+StringI+structNameRail).c_str();
        shader->setUniformValue((structNameFront+StringI+structNameRail).c_str(),constant);
    }
}

void LightData::setLinear(float linear){
//    if(!pointLightActivated){
//        qDebug()<<"PointLight is not activated";
//        return;
//    }
    std::string structNameFront = "pointLights[";
    std::string structNameRail = "].linear";
    for(int i = 0; i < numPointLight; i++){
        std::string StringI = std::to_string(i);
//        qDebug()<<(structNameFront+StringI+structNameRail).c_str();
        shader->setUniformValue((structNameFront+StringI+structNameRail).c_str(),linear);
    }
}

void LightData::setQuadratic(float quadratic){
//    if(!pointLightActivated){
//        qDebug()<<"PointLight is not activated";
//        return;
//    }
    std::string structNameFront = "pointLights[";
    std::string structNameRail = "].quadratic";
    for(int i = 0; i < numPointLight; i++){
        std::string StringI = std::to_string(i);
//        qDebug()<<(structNameFront+StringI+structNameRail).c_str();
        shader->setUniformValue((structNameFront+StringI+structNameRail).c_str(),quadratic);
    }
}
