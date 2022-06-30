#include "shaderSelector.h"

ShaderSelector::ShaderSelector(){
}

void ShaderSelector::compileShader(int shaderType){
    if(shaderType < 0||shaderType > vertexPath.length()){
        qDebug()<<"shaderType out of bounds";
        return;
    }
        QOpenGLShaderProgram* shader = new QOpenGLShaderProgram();
        shader->addShaderFromSourceFile(QOpenGLShader::Vertex,vertexPath[shaderType]);
        shader->addShaderFromSourceFile(QOpenGLShader::Fragment,fragmentPath[shaderType]);
        shader->link();
        qDebug()<<"compile shader"<<shaderType<<vertexPath[shaderType];
        shaderProgram.push_back(shader);

}

QOpenGLShaderProgram* ShaderSelector::getShader(int objNum){
    if(shaderProgram.isEmpty()){
        qDebug()<<"shaderProgram is empty";
    }
    if(objNum < 0 || objNum > shaderProgram.size()){
        qDebug()<<"haven't assign a shader to object";
    }
    return shaderProgram[objNum];
}
