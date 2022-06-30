#include "lightData.h"

LightData::LightData(QOpenGLShaderProgram* shaderProgram):shader(shaderProgram){
};

void LightData::activateDirLight(){
    dirLightActivated = true;
    shader->setUniformValue("dirLightActivated",true);
}

void LightData::disableDirLight()
{
    dirLightActivated = false;
    shader->setUniformValue("dirLightActivated",false);
}
void LightData::setDirLightDirection(QVector3D& direction){
    if(!dirLightActivated){
        qDebug()<<"DirLight is not activated";
        return;
    }
    shader->setUniformValue("dirLight.direction",direction);
}

void LightData::setDirLightAmbientColor(QVector3D Color,float ambient){
    if(!dirLightActivated){
        qDebug()<<"DirLight is not activated";
        return;
    }
    shader->setUniformValue("dirLight.ambient",Color * ambient);

}

void LightData::setDirLightDiffuseColor(QVector3D Color, float diffuse){
    if(!dirLightActivated){
        qDebug()<<"DirLight is not activated";
        return;
    }
    shader->setUniformValue("dirLight.diffuse",Color * diffuse);
}
void LightData::setDirLightSpecularColor(QVector3D Color, float specular){
    if(!dirLightActivated){
        qDebug()<<"DirLight is not activated";
        return;
    }
    shader->setUniformValue("dirLight.specular",Color * specular);
}

void LightData::activatePointLight(){
    pointLightActivated = true;
    shader->setUniformValue("pointLightActivated",true);
}

void LightData::disablepointLight()
{
    pointLightActivated = false;
    shader->setUniformValue("pointLightActivated",false);
}

void LightData::setPointLightPosition(QVector<QVector3D>& position){
    if(!pointLightActivated){
        qDebug()<<"PointLight is not activated";
        return;
    }
    numPointLight = position.size();
    shader->setUniformValue("numPointLights",numPointLight);

    std::string structNameFront = "pointLights[";
    std::string structNameRail = "].position";
    qDebug()<<"numPoints"<<numPointLight;
    for(int i = 0; i < numPointLight; i++){
        std::string StringI = std::to_string(i);
//        qDebug()<<(structNameFront+StringI+structNameRail).c_str();
        shader->setUniformValue((structNameFront+StringI+structNameRail).c_str(),position[i]);
    }
}

void LightData::setPointLightAmbientColor(QVector<QVector3D> Color,float ambient){
    if(!pointLightActivated){
        qDebug()<<"PointLight is not activated";
        return;
    }
    std::string structNameFront = "pointLights[";
    std::string structNameRail = "].ambient";
    for(int i = 0; i < numPointLight; i++){
        std::string StringI = std::to_string(i);
//        qDebug()<<(structNameFront+StringI+structNameRail).c_str();
        shader->setUniformValue((structNameFront+StringI+structNameRail).c_str(),Color[i] * ambient);
    }
}

void LightData::setPointLightDiffuseColor(QVector<QVector3D> Color, float diffuse){
    if(!pointLightActivated){
        qDebug()<<"PointLight is not activated";
        return;
    }
    std::string structNameFront = "pointLights[";
    std::string structNameRail = "].diffuse";
    for(int i = 0; i < numPointLight; i++){
        std::string StringI = std::to_string(i);
//        qDebug()<<(structNameFront+StringI+structNameRail).c_str();
        shader->setUniformValue((structNameFront+StringI+structNameRail).c_str(),Color[i] * diffuse);
    }
}

void LightData::setPointLightSpecularColor(QVector<QVector3D> Color, float specular){
    if(!pointLightActivated){
        qDebug()<<"PointLight is not activated";
        return;
    }
    std::string structNameFront = "pointLights[";
    std::string structNameRail = "].specular";
    for(int i = 0; i < numPointLight; i++){
        std::string StringI = std::to_string(i);
//        qDebug()<<(structNameFront+StringI+structNameRail).c_str();
        shader->setUniformValue((structNameFront+StringI+structNameRail).c_str(),Color[i] * specular);
    }
}

void LightData::setConstant(float constant){
    if(!pointLightActivated){
        qDebug()<<"PointLight is not activated";
        return;
    }
    std::string structNameFront = "pointLights[";
    std::string structNameRail = "].constant";
    for(int i = 0; i < numPointLight; i++){
        std::string StringI = std::to_string(i);
//        qDebug()<<(structNameFront+StringI+structNameRail).c_str();
        shader->setUniformValue((structNameFront+StringI+structNameRail).c_str(),constant);
    }
}

void LightData::setLinear(float linear){
    if(!pointLightActivated){
        qDebug()<<"PointLight is not activated";
        return;
    }
    std::string structNameFront = "pointLights[";
    std::string structNameRail = "].linear";
    for(int i = 0; i < numPointLight; i++){
        std::string StringI = std::to_string(i);
//        qDebug()<<(structNameFront+StringI+structNameRail).c_str();
        shader->setUniformValue((structNameFront+StringI+structNameRail).c_str(),linear);
    }
}

void LightData::setQuadratic(float quadratic){
    if(!pointLightActivated){
        qDebug()<<"PointLight is not activated";
        return;
    }
    std::string structNameFront = "pointLights[";
    std::string structNameRail = "].quadratic";
    for(int i = 0; i < numPointLight; i++){
        std::string StringI = std::to_string(i);
//        qDebug()<<(structNameFront+StringI+structNameRail).c_str();
        shader->setUniformValue((structNameFront+StringI+structNameRail).c_str(),quadratic);
    }
}
