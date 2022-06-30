#ifndef SHADERSELECTOR_H
#define SHADERSELECTOR_H

#include <QVector>
#include <QString>
#include <QOpenGLShaderProgram>
#include <QDebug>

class ShaderSelector
{
public:
    QVector<QString> vertexPath{
        ":/texture.vert",
        ":/lightObject.vert",
        ":/color.vert"
    };
    QVector<QString> fragmentPath{
        ":/texture.frag",
        ":/lightObject.frag",
        ":color.frag"
    };
    ShaderSelector();
    QOpenGLShaderProgram* getShader(int objNum);
    void compileShader(int shaderType = 2);
private:
    QVector<QOpenGLShaderProgram*> shaderProgram;

};

#endif // SHADERSELECTOR_H
