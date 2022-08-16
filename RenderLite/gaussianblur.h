#ifndef GAUSSIANBLUR_H
#define GAUSSIANBLUR_H

#include <QOpenGLFunctions>
#include <QOpenGLFramebufferObject>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>


class GaussianBlur : public QOpenGLFunctions
{
private:
    //-----------------¸ßË¹Ä£ºý
    QOpenGLFramebufferObject *GaussionBlur[2];
    QOpenGLShaderProgram *GaussianBlurShader;

    QOpenGLVertexArrayObject quadVAO;
    QOpenGLBuffer quadVBO;


public:
    GaussianBlur();

    unsigned int getGaussBlurPhoto(unsigned int BlurId,int w,int h,unsigned int amount = 10);
    void renderQuad();
    ~GaussianBlur();
};

#endif // GAUSSIANBLUR_H
