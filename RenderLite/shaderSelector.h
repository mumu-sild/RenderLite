#ifndef SHADERSELECTOR_H
#define SHADERSELECTOR_H

#include <QVector>
#include <QString>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>
#include <QDebug>

#include "dirLight.h"
#include "pointLight.h"

class GLWidget;

class ShaderSelector : public QOpenGLFunctions
{
public:
    QVector<QString> vertexPath{
        ":/texture.vert",
        ":/lightObject.vert",
        ":/color.vert",
        ":/lightColor.vert",
        ":/frameShader.vert"
    };
    QVector<QString> fragmentPath{
        ":/texture.frag",
        ":/lightObject.frag",
        ":color.frag",
        ":/lightColor.frag",
        ":/frameShader.frag"
    };
    ShaderSelector();
    ~ShaderSelector();
public:
    QOpenGLShaderProgram* getShader(int shaderNum=2);
    void compileShader(int i);
    void setLightDir(int shader,DirLight* dirlight);
    void setPointDir(int shader,QVector<PointLight*> pointlights);

    void OpenGLFunctionsInit();

private:
    QVector<QOpenGLShaderProgram*> shaderProgram;

};

#endif // SHADERSELECTOR_H
