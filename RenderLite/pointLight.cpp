#include "pointLight.h"

float PointLight::ambient = pointLightDefaultAmbient;
float PointLight::diffuse = PointLightDefaultDiffuse;
float PointLight::specular = PointLightDefaultSpecular;
float PointLight::constant = PointLightDefaultConstant;
float PointLight::linear = PointLightDefaultLinear;
float PointLight::quadratic=PointLightDefaultQuadratic;

QMatrix4x4 PointLight::getLightMatrix()
{
    QMatrix4x4 lightProjection, lightView;

    float near_plane = 0.50f, far_plane = 100.5f;
    lightProjection.perspective(150.0f,1,near_plane,far_plane);
    //lightProjection.ortho(-eyeing, eyeing, -eyeing, eyeing, near_plane, far_plane);
    lightView.lookAt(position, position+lightNormal, QVector3D(0.0, 1.0, 0.0));
    lightSpaceMatrix = lightProjection * lightView;
    return lightSpaceMatrix;
}

void PointLight::setShaderPara(QOpenGLShaderProgram *shader,int UniformInt,int i)
{
    QString structNameFront = "pointLights[";
    QString StringNum;
    StringNum.setNum(i);
    QString StringI = structNameFront+StringNum;
    shader->setUniformValue(QString(StringI+"].position").toStdString().c_str(),position);
    shader->setUniformValue(QString(StringI+"].ambient").toStdString().c_str(),getColor() * PointLight::ambient);
    shader->setUniformValue(QString(StringI+"].diffuse").toStdString().c_str(),getColor() * PointLight::diffuse);
    shader->setUniformValue(QString(StringI+"].specular").toStdString().c_str(),getColor() * PointLight::specular);
    shader->setUniformValue(QString(StringI+"].lightnormal").toStdString().c_str(),lightNormal);
    shader->setUniformValue(QString(StringI+"].constant").toStdString().c_str(),PointLight::constant);
    shader->setUniformValue(QString(StringI+"].linear").toStdString().c_str(),PointLight::linear);
    shader->setUniformValue(QString(StringI+"].quadratic").toStdString().c_str(),PointLight::quadratic);
    shader->setUniformValue(QString(StringI+"].shadowMap").toStdString().c_str(),UniformInt);
    shader->setUniformValue(QString(StringI+"].lightSpaceMatrix").toStdString().c_str(),getLightMatrix());
    shader->setUniformValue(QString(StringI+"].width").toStdString().c_str(),width);

    //qDebug()<<"width="<<pointlights[i]->width;
    glActiveTexture(GL_TEXTURE0+UniformInt);
    glBindTexture(GL_TEXTURE_2D,depthMapFBO->texture());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

}

void PointLight::setPosition(const QVector3D &newPosition)
{
    position = newPosition;
}

QVector3D &PointLight::getPosition()
{
    return position;
}

void PointLight::setPositionX(const float x)
{
    position.setX(x);
}

void PointLight::setPositionY(const float y)
{
    position.setY(y);
}

void PointLight::setPositionZ(const float z)
{
    position.setZ(z);
}

QVector3D PointLight::getColor()
{
    return color*intensity;
}

void PointLight::setColorR(const float r)
{
    color.setX(r);
}
void PointLight::setColorG(const float g)
{
    color.setX(g);
}
void PointLight::setColorB(const float b)
{
    color.setX(b);
}



