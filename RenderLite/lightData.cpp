#include "lightData.h"

LightData::LightData(QOpenGLShaderProgram* shaderProgram):shader(shaderProgram){};

void LightData::activateDirLight(){
    dirLightActivated = true;
    shader->setUniformValue("dirLightActivated",true);
}
void LightData::setDirLightDirection(QVector3D& direction){
    if(!dirLightActivated){
        qDebug()<<"DirLight is not activated";
        return;
    }
    shader->setUniformValue("dirLight.direction",direction);
}

void LightData::setDirLightAmbient(QVector3D& ambient){
    if(!dirLightActivated){
        qDebug()<<"DirLight is not activated";
        return;
    }
    shader->setUniformValue("dirLight.ambient",ambient);
}

void LightData::setDirLightDiffuse(QVector3D& diffuse){
    if(!dirLightActivated){
        qDebug()<<"DirLight is not activated";
        return;
    }
    shader->setUniformValue("dirLight.diffuse",diffuse);
}
void LightData::setDirLightSpecular(QVector3D& specular){
    if(!dirLightActivated){
        qDebug()<<"DirLight is not activated";
        return;
    }
    shader->setUniformValue("dirLight.specular",specular);
}

void LightData::activatePointLight(){
    pointLightActivated = true;
    shader->setUniformValue("pointLightActivated",true);
}

void LightData::setPointLightPosition(QVector3D* position){
    if(!pointLightActivated){
        qDebug()<<"PointLight is not activated";
        return;
    }
    numPointLight = sizeof(position);
    shader->setUniformValue("numPointLights",numPointLight);

    std::string structNameFront = "pointLights[";
    std::string structNameRail = "].position";
//    qDebug()<<"numPoints"<<numPointLight;
    for(int i = 0; i < numPointLight; i++){
        std::string StringI = std::to_string(i);
//        qDebug()<<(structNameFront+StringI+structNameRail).c_str();
        shader->setUniformValue((structNameFront+StringI+structNameRail).c_str(),position[i]);
    }
}

void LightData::setPointLightAmbient(QVector3D& ambient){
    if(!pointLightActivated){
        qDebug()<<"PointLight is not activated";
        return;
    }
    std::string structNameFront = "pointLights[";
    std::string structNameRail = "].ambient";
    for(int i = 0; i < numPointLight; i++){
        std::string StringI = std::to_string(i);
//        qDebug()<<(structNameFront+StringI+structNameRail).c_str();
        shader->setUniformValue((structNameFront+StringI+structNameRail).c_str(),ambient);
    }
}

void LightData::setPointLightDiffuse(QVector3D& diffuse){
    if(!pointLightActivated){
        qDebug()<<"PointLight is not activated";
        return;
    }
    std::string structNameFront = "pointLights[";
    std::string structNameRail = "].diffuse";
    for(int i = 0; i < numPointLight; i++){
        std::string StringI = std::to_string(i);
//        qDebug()<<(structNameFront+StringI+structNameRail).c_str();
        shader->setUniformValue((structNameFront+StringI+structNameRail).c_str(),diffuse);
    }
}

void LightData::setPointLightSpecular(QVector3D& specular){
    if(!pointLightActivated){
        qDebug()<<"PointLight is not activated";
        return;
    }
    std::string structNameFront = "pointLights[";
    std::string structNameRail = "].specular";
    for(int i = 0; i < numPointLight; i++){
        std::string StringI = std::to_string(i);
//        qDebug()<<(structNameFront+StringI+structNameRail).c_str();
        shader->setUniformValue((structNameFront+StringI+structNameRail).c_str(),specular);
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
