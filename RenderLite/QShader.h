#ifndef QSHADER_H
#define QSHADER_H

#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <QOpenGLContext>
#include <qopenglfunctions.h>
#include <QVector3D>
#include <QMatrix4x4>
#include <QMatrix3x3>

typedef unsigned int GLunit;
typedef char GLchar;
typedef int GLint;

class QShader
{
public:
    unsigned ID;
    QShader(const char* vertexPath, const char* fragmentPath, const char* geometryPath, QOpenGLFunctions* f);

    void use();
    void release();

    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;

    void setVec2(const std::string& name,float x,float y)const;

    void setVec3(const std::string& name,float x,float y, float z)const;
    void setVec3(const std::string& name,QVector3D value) const;
    void setVec4(const std::string& name,float x,float y,float z,float w)const;
    void setMat4(const std::string& name,QMatrix4x4 value) const;
    void setMat3(const std::string& name,QMatrix3x3 value) const;

    GLuint CreateFragmentShaders(const GLchar* ShaderSource);
    GLuint CreateVertexShaders(const GLchar* ShaderSource);
    GLuint CreateGeometryShaders(const GLchar* ShaderSource);

    void checkCompileErrors(GLunit shader);

    QOpenGLFunctions* f;


};

#endif // QSHADER_H
