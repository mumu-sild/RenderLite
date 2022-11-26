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
    GaussianBlur(int w, int h);
    void resizeGaussBlurFBO(int w,int h);
    QOpenGLFramebufferObject* getGaussBlurPhoto(unsigned int BlurId,unsigned int amount = 10);
    void renderQuad();
    ~GaussianBlur();
};

#endif // GAUSSIANBLUR_H
