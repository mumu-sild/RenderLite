#include "shaderSelector.h"
#include "lightData.h"

ShaderSelector::ShaderSelector(){}

ShaderSelector::~ShaderSelector()
{
    for(int i=0;i<shaderProgram.size();++i){
        delete shaderProgram.at(i);
    }
}

void ShaderSelector::setLightDir(int i, DirLight *dirlight)
{
//    struct DirLight {
//            bool Activated;
//            vec3 direction;
//
//            vec3 ambient;
//            vec3 diffuse;
//            vec3 specular;
//    }dirLight;
    QOpenGLShaderProgram *shader = getShader(i);
    shader->setUniformValue("dirLight.Activated",dirlight->dirLightActivated);
    shader->setUniformValue("dirLight.direction",dirlight->getDirection());
    shader->setUniformValue("dirLight.ambient",dirlight->getColor()*DirLight::ambient);
    shader->setUniformValue("dirLight.diffuse",dirlight->getColor()*DirLight::diffuse);
    shader->setUniformValue("dirLight.specular",dirlight->getColor()*DirLight::specular);
    return;
}

void ShaderSelector::setPointDir(int j, QVector<PointLight *> pointlights)
{
//    struct PointLight {
//            vec3 position;

//            vec3 ambient;
//            vec3 diffuse;
//            vec3 specular;

//            vec3 lightnormal;

//            float constant;
//            float linear;
//            float quadratic;

//    }pointLights[16];
    QOpenGLShaderProgram *shader = getShader(j);
    int numPointLight = pointlights.size();
    shader->setUniformValue("numPointLights",numPointLight);
    QString structNameFront = "pointLights[";
    for(int i = 0; i < numPointLight; i++){
        QString StringNum;
        StringNum.setNum(i);
        QString StringI = structNameFront+StringNum;
        shader->setUniformValue(QString(StringI+"].position").toStdString().c_str(),
                                pointlights[i]->position);
        shader->setUniformValue(QString(StringI+"].ambient").toStdString().c_str(),
                                pointlights[i]->color * PointLight::ambient);
        shader->setUniformValue(QString(StringI+"].diffuse").toStdString().c_str(),
                                pointlights[i]->color * PointLight::diffuse);
        shader->setUniformValue(QString(StringI+"].specular").toStdString().c_str(),
                                pointlights[i]->color * PointLight::specular);
        shader->setUniformValue(QString(StringI+"].lightnormal").toStdString().c_str(),
                                pointlights[i]->lightNormal);
        shader->setUniformValue(QString(StringI+"].constant").toStdString().c_str(),
                                PointLight::constant);
        shader->setUniformValue(QString(StringI+"].linear").toStdString().c_str(),
                                PointLight::linear);
        shader->setUniformValue(QString(StringI+"].quadratic").toStdString().c_str(),
                                PointLight::quadratic);
    }
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
    qDebug()<<"compile shader"<<i<<":"<<vertexPath[i]<<"success";
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
