#ifndef LIGHTDATA_H
#define LIGHTDATA_H

#include <QVector3D>
#include <QOpenGLShaderProgram>
#include <string>
#include <QDebug>

class LightData
{
public:
    LightData(QOpenGLShaderProgram* shaderProgram);

    void activateDirLight();
    void setDirLightDirection(QVector3D& direction);
    void setDirLightAmbient(QVector3D& ambient);
    void setDirLightDiffuse(QVector3D& diffuse);
    void setDirLightSpecular(QVector3D& specular);

    void activatePointLight();
    void setPointLightPosition(QVector3D* position);
    void setPointLightAmbient(QVector3D& ambient);
    void setPointLightDiffuse(QVector3D& diffuse);
    void setPointLightSpecular(QVector3D& specular);
    void setConstant(float constant);
    void setLinear(float linear);
    void setQuadratic(float quadratic);
private:
    QOpenGLShaderProgram* shader = nullptr;
    //parallel lights attribs
    bool dirLightActivated = false;

    //point lights attribs
    bool pointLightActivated = false;
    int numPointLight = -1;
    QVector3D* pointLightPosition;
    const static int MAX_POINT_LIGHT = 16;

};

#endif // LIGHTDATA_H
