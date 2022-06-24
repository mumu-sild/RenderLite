#ifndef QSHADER_H
#define QSHADER_H

#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <QOpenGLContext>
#include <qopenglfunctions.h>
#include <QMatrix4x4>
#include <QMatrix3x3>
#include <QFile>
#include "vec.h"

typedef unsigned int GLunit;
typedef char GLchar;
typedef int GLint;

class QShader : protected QOpenGLFunctions
{
public:
    unsigned ID;
    QShader(QString& vertexPath, QString& fragmentPath, QString& geometryPath);

    void use();
    void release();

    void setBool(const std::string& name, bool value) ;
    void setInt(const std::string& name, int value) ;
    void setFloat(const std::string& name, float value) ;

    void setVec2(const std::string& name,float x,float y);

    void setVec3(const std::string& name,float x,float y, float z);
    void setVec3(const std::string& name,vec3 value) ;
    void setVec4(const std::string& name,float x,float y,float z,float w);
    void setMat4(const std::string& name,QMatrix4x4 value) ;
    void setMat3(const std::string& name,QMatrix3x3 value) ;

    GLuint CreateFragmentShaders(const GLchar* ShaderSource);
    GLuint CreateVertexShaders(const GLchar* ShaderSource);
    GLuint CreateGeometryShaders(const GLchar* ShaderSource);

    void checkCompileErrors(GLunit shader);



};

#endif // QSHADER_H
