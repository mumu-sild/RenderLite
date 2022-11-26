#include "gaussianblur.h"


GaussianBlur::GaussianBlur(int w, int h) : quadVBO(QOpenGLBuffer::VertexBuffer)
{
    initializeOpenGLFunctions();
    //shader
    GaussianBlurShader = new QOpenGLShaderProgram();
    GaussianBlurShader->addShaderFromSourceFile(QOpenGLShader::Vertex,":/map_depth.vert");
    GaussianBlurShader->addShaderFromSourceFile(QOpenGLShader::Fragment,":/GaussianBlur.frag");
    GaussianBlurShader->link();

    GaussionBlur[0] = new QOpenGLFramebufferObject(w,h,QOpenGLFramebufferObject::NoAttachment,GL_TEXTURE_2D,GL_RGBA16F);
    GaussionBlur[1] = new QOpenGLFramebufferObject(w,h,QOpenGLFramebufferObject::NoAttachment,GL_TEXTURE_2D,GL_RGBA16F);

    //quadVAO
    const float quadVertices[] = {
        // positions        // texture Coords
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
         1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
         1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    };
    quadVAO.create();
    quadVAO.bind();
    quadVBO.create();
    quadVBO.bind();
    quadVBO.allocate(quadVertices,sizeof(quadVertices));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    quadVAO.release();
}

void GaussianBlur::resizeGaussBlurFBO(int w, int h)
{
    delete GaussionBlur[0];
    delete GaussionBlur[1];
    GaussionBlur[0] = new QOpenGLFramebufferObject(w,h,QOpenGLFramebufferObject::NoAttachment,GL_TEXTURE_2D,GL_RGBA16F);
    GaussionBlur[1] = new QOpenGLFramebufferObject(w,h,QOpenGLFramebufferObject::NoAttachment,GL_TEXTURE_2D,GL_RGBA16F);
}

QOpenGLFramebufferObject* GaussianBlur::getGaussBlurPhoto(unsigned int BlurId, unsigned int amount)
{
    bool horizontal = true;
    bool first_iteration = true;
    //¸ßË¹Ä£ºý»º³å

    GaussianBlurShader->bind();


    for (GLuint i = 0; i < amount*2; i++)
    {
        if(horizontal)GaussionBlur[0]->bind();
        else GaussionBlur[1]->bind();
        glClear(GL_COLOR_BUFFER_BIT);
        GaussianBlurShader->setUniformValue("horizontal",horizontal);
        GaussianBlurShader->setUniformValue("image",0);
        glActiveTexture(GL_TEXTURE0);
        unsigned int id = 0;
        if(first_iteration){
            id = BlurId;
        }
        else{
            if(horizontal)id = GaussionBlur[1]->texture();
            else id = GaussionBlur[0]->texture();
        }
        glBindTexture(GL_TEXTURE_2D, id);
        renderQuad();
        horizontal = !horizontal;
        if (first_iteration)
            first_iteration = false;
    }
    return GaussionBlur[1];
}

void GaussianBlur::renderQuad()
{
    quadVAO.bind();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    quadVAO.release();
}

GaussianBlur::~GaussianBlur()
{
    delete GaussianBlurShader;
    delete GaussionBlur[0];
    delete GaussionBlur[1];
}
