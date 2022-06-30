#include "shaderSelector.h"

ShaderSelector::ShaderSelector(){}

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
