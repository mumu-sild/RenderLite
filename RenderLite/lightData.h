#ifndef LIGHTDATA_H
#define LIGHTDATA_H

#include <QVector3D>
#include <QOpenGLShaderProgram>
#include <string>
#include <QDebug>
#include "pointLight.h"

class LightData
{
public:
    LightData(QOpenGLShaderProgram* shaderProgram);

    void activateDirLight();
    void disableDirLight();
    void setDirLightDirection(QVector3D& direction);
    void setDirLightAmbientColor(QVector3D Color,float ambient);
    void setDirLightDiffuseColor(QVector3D Color, float diffuse);
    void setDirLightSpecularColor(QVector3D Color, float specular);

    void activatePointLight(QVector<PointLight *> &pointLight);
    void disablepointLight(QVector<PointLight *> &pointLight);
    void setPointLightColor(QVector<PointLight*>&pointLight);
    void setPointLightPosition(QVector<PointLight*>& pointLight);
    void setPointLightAmbientColor(QVector<PointLight*> pointLight,float ambient);
    void setPointLightDiffuseColor(QVector<PointLight*> pointLight,float diffuse);
    void setPointLightSpecularColor(QVector<PointLight*> pointLight, float specular);
    void setPointLightNormal(QVector<PointLight*> pointLight);
    void setConstant(float constant);
    void setLinear(float linear);
    void setQuadratic(float quadratic);
private:
    QOpenGLShaderProgram* shader = nullptr;
    //parallel lights attribs
//    bool dirLightActivated = false;

    //point lights attribs
//    bool pointLightActivated = false;
    int numPointLight = -1;
    QVector3D* pointLightPosition;
    const static int MAX_POINT_LIGHT = 16;

};

#endif // LIGHTDATA_H
