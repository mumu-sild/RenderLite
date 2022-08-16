#include "dirLight.h"

float DirLight::ambient = DirLightDefaultAmbient;
float DirLight::diffuse = DirLightDefaultDiffuse;
float DirLight::specular = DirLightDefaultSpecular;

QVector3D &DirLight::getDirection()
{
    return direction;
}

void DirLight::setDirectionX(const float x)
{
    direction.setX(x);
}

void DirLight::setDirectionY(const float y)
{
    direction.setY(y);
}

void DirLight::setDirectionZ(const float z)
{
    direction.setZ(z);
}

QVector3D &DirLight::getColor()
{
    return color;
}

void DirLight::setColorR(const float r)
{
    color.setX(r);
}

void DirLight::setColorG(const float g)
{
    color.setY(g);
}

void DirLight::setColorB(const float b)
{
    color.setZ(b);
}

QMatrix4x4 DirLight::getLightMatrix()
{
    QVector3D lightPos = -direction.normalized()*50;
    QMatrix4x4 lightProjection, lightView;

    float near_plane = -50.50f, far_plane = 100.5f;
    const float eyeing = 100.0f;
    lightProjection.ortho(-eyeing, eyeing, -eyeing, eyeing, near_plane, far_plane);
    lightView.lookAt(lightPos, QVector3D(0,0,0), QVector3D(0.0, 1.0, 0.0));
    lightSpaceMatrix = lightProjection * lightView;
    return lightSpaceMatrix;
}

void DirLight::setShaderPara(QOpenGLShaderProgram *shader,int UniformInt)
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
    shader->setUniformValue("dirLight.Activated",Activated);
    shader->setUniformValue("dirLight.direction",getDirection());
    shader->setUniformValue("dirLight.ambient",getColor()*DirLight::ambient);
    shader->setUniformValue("dirLight.diffuse",getColor()*DirLight::diffuse);
    shader->setUniformValue("dirLight.specular",getColor()*DirLight::specular);
    //注意，对于有纹理的阴影生成，不能将阴影图的绑定点与纹理绑定点相同****
    shader->setUniformValue("dirLight.shadowMap",UniformInt);
    shader->setUniformValue("dirLight.lightSpaceMatrix",getLightMatrix());

    glActiveTexture(GL_TEXTURE0+UniformInt);
    glBindTexture(GL_TEXTURE_2D,depthMapFBO->texture());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

}
