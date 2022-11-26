#ifndef SSAO_H
#define SSAO_H

#include <QOpenGLFunctions>
#include <QOpenGLFramebufferObject>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>


class SSAO : public QOpenGLFunctions
{
public:
    QOpenGLFramebufferObject* SSAOFBO;
    QOpenGLShaderProgram *SSAOShader,*debugShader;
    QVector<QVector3D> SSAOKernel;
    QOpenGLTexture texture;

    bool debug = false;

public:
    SSAO(int w,int h);

    void generateKernel();
    void generateNoise();//随机核心转动
    void setUniform(QOpenGLFramebufferObject* GBuffer,QMatrix4x4 projection,QMatrix4x4 view);
    QOpenGLFramebufferObject* SSAOPicture(QOpenGLFramebufferObject* GBuffer,QMatrix4x4 projection,QMatrix4x4 view);
    void renderQuad();
    void resizeSSAO(int w,int h);

    ~SSAO();
};


#endif // SSAO_H
