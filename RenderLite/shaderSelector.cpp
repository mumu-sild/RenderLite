#include "shaderSelector.h"

ShaderSelector::ShaderSelector(){
//    initializeOpenGLFunctions();必须在OpenGLWidget创建时候使用，否则会crash
}

ShaderSelector::~ShaderSelector()
{
    for(int i=0;i<shaderProgram.size();++i){
        delete shaderProgram.at(i);
    }
}

void ShaderSelector::setLightDir(int i, DirLight *dirlight)
{
//    struct DirLight {
//        bool Activated;
//        vec3 direction;

//        vec3 ambient;
//        vec3 diffuse;
//        vec3 specular;
//        //阴影
//        sampler2D shadowMap;
//        mat4 lightSpaceMatrix;
//    };
    QOpenGLShaderProgram *shader = getShader(i);
    shader->setUniformValue("dirLight.Activated",dirlight->Activated);
    shader->setUniformValue("dirLight.direction",dirlight->getDirection());
    shader->setUniformValue("dirLight.ambient",dirlight->getColor()*DirLight::ambient);
    shader->setUniformValue("dirLight.diffuse",dirlight->getColor()*DirLight::diffuse);
    shader->setUniformValue("dirLight.specular",dirlight->getColor()*DirLight::specular);
    //注意，对于有纹理的阴影生成，不能将阴影图的绑定点与纹理绑定点相同****
    shader->setUniformValue("dirLight.shadowMap",3);
    shader->setUniformValue("dirLight.lightSpaceMatrix",dirlight->getLightMatrix());

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D,dirlight->depthMapFBO->texture());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
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

//            //阴影
//            sampler2D shadowMap;
//            mat4 lightSpaceMatrix;
//            float width;

//    }pointLights[16];
    QOpenGLShaderProgram *shader = getShader(j);
    int numPointLight = pointlights.size();
    shader->setUniformValue("numPointLights",numPointLight);
    QString structNameFront = "pointLights[";
    for(int i = 0; i < numPointLight; i++){
        QString StringNum;
        StringNum.setNum(i);
        QString StringI = structNameFront+StringNum;
        shader->setUniformValue(QString(StringI+"].position").toStdString().c_str(),pointlights[i]->position);
        shader->setUniformValue(QString(StringI+"].ambient").toStdString().c_str(),pointlights[i]->getColor() * PointLight::ambient);
        shader->setUniformValue(QString(StringI+"].diffuse").toStdString().c_str(),pointlights[i]->getColor() * PointLight::diffuse);
        shader->setUniformValue(QString(StringI+"].specular").toStdString().c_str(),pointlights[i]->getColor() * PointLight::specular);
        shader->setUniformValue(QString(StringI+"].lightnormal").toStdString().c_str(),pointlights[i]->lightNormal);
        shader->setUniformValue(QString(StringI+"].constant").toStdString().c_str(),PointLight::constant);
        shader->setUniformValue(QString(StringI+"].linear").toStdString().c_str(),PointLight::linear);
        shader->setUniformValue(QString(StringI+"].quadratic").toStdString().c_str(),PointLight::quadratic);
        shader->setUniformValue(QString(StringI+"].shadowMap").toStdString().c_str(), 4+i);
        shader->setUniformValue(QString(StringI+"].lightSpaceMatrix").toStdString().c_str(),pointlights[i]->getLightMatrix());
        shader->setUniformValue(QString(StringI+"].width").toStdString().c_str(),pointlights[i]->width);

        //qDebug()<<"width="<<pointlights[i]->width;

        glActiveTexture(GL_TEXTURE4+i);
        glBindTexture(GL_TEXTURE_2D,pointlights[i]->depthMapFBO->texture());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    }

}

void ShaderSelector::OpenGLFunctionsInit()
{
     initializeOpenGLFunctions();
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
