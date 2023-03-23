/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QMouseEvent>
#include <QOpenGLShaderProgram>
#include <QOpenGLFramebufferObject>
#include <QCoreApplication>
#include <math.h>
#include <QDir>
#include <QString>
#include <iostream>
#include <QDebug>
#include <QOpenGLFunctions>
#include <QOpenGLFunctions_3_3_Core>


#include "glwidget.h"
#include "Global.h"
#include "camera.h"
#include "Setting.h"

#include "rectangle.h"
#include "triangle.h"
#include "Model.h"
#include "sphere.h"

//#ifndef STB__
//#define STB__
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
//#endif


GLWidget::GLWidget(QWidget *parent)
    : QOpenGLWidget(parent)
{
    this->setFocusPolicy(Qt::StrongFocus);
    QSurfaceFormat formate;
    //formate.setSamples(4);
    auto major = formate.majorVersion();
    auto minor = formate.minorVersion();
    qDebug() << "OpenGL Version Major:" << major << "OpenGL Version Minor:" << minor;

    setFormat(formate);
}

GLWidget::~GLWidget()
{
    cleanup();
}

QSize GLWidget::minimumSizeHint() const
{
    return QSize(windowsMinimumSize, windowsMinimumSize);
}

QSize GLWidget::sizeHint() const
{
    return QSize(windowsNormalSize, windowsNormalSize);
}

void GLWidget::cleanup()
{
    makeCurrent();

    doneCurrent();
}

void GLWidget::initializeGL()
{
    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &GLWidget::cleanup);

    initializeOpenGLFunctions();
    shaderSelector.OpenGLFunctionsInit();
    core = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();
    //gaussBlur = new GaussianBlur(width(),height());
    //ssao = new SSAO(width(),height());

    glClearColor(backgroundDefaultColor.x(),
                 backgroundDefaultColor.y(),
                 backgroundDefaultColor.z(), 1);

    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    glEnable(GL_STENCIL_TEST);
    glDepthFunc(GL_LEQUAL);

    debug_dep = new QOpenGLShaderProgram();
    debug_dep->addShaderFromSourceFile(QOpenGLShader::Vertex,":/map_depth.vert");
    debug_dep->addShaderFromSourceFile(QOpenGLShader::Fragment,":/map_depth.frag");
    debug_dep->link();

    //加载天空盒
    vector<std::string> faces
    {
        "../RenderLite/Picture_source/skybox/right.jpg",
        "../RenderLite/Picture_source/skybox/left.jpg",
        "../RenderLite/Picture_source/skybox/top.jpg",
        "../RenderLite/Picture_source/skybox/bottom.jpg",
        "../RenderLite/Picture_source/skybox/front.jpg",
        "../RenderLite/Picture_source/skybox/back.jpg"
    };
    cubemapTexture = loadCubemap(faces);

//IBL

    //1. 加载图像  Alexs_Apt_8k.jpg
//    QImage data("C:/Users/mumu/Desktop/graphics/RenderLite/outerFile/Alexs_Apartment/Alexs_Apartment/Alexs_Apt_8k.jpg");
//    if(data.isNull()){
//        qDebug()<<"data is null;";
//    }
//    else{
//        data = data.mirrored();
//        equirectangularMap = new QOpenGLTexture(data);
//        equirectangularMap->create();
//        equirectangularMap->setWrapMode(QOpenGLTexture::DirectionS,QOpenGLTexture::Repeat);
//        equirectangularMap->setWrapMode(QOpenGLTexture::DirectionT,QOpenGLTexture::Repeat);
//        equirectangularMap->setMinMagFilters(QOpenGLTexture::LinearMipMapLinear,QOpenGLTexture::Linear);
//    }

    //          Alexs_Apt_2k.hdr
    stbi_set_flip_vertically_on_load(true);

    float *data2 = stbi_loadf("C:/Users/mumu/Desktop/graphics/RenderLite/outerFile/Alexs_Apartment/Alexs_Apartment/Alexs_Apt_8k.jpg"\
                            , &Texwidth, &Texheight, &TexnrComponents, 0);
    if (data2)
    {
        glGenTextures(1, &hdrTexture);
        glBindTexture(GL_TEXTURE_2D, hdrTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, Texwidth, Texheight, 0, GL_RGB, GL_FLOAT, data2);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data2);
    }
    else
    {
        std::cout << "Failed to load HDR image." << std::endl;
    }

    //FBO 创建帧缓存、绑定深度缓存和模板缓存
    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

    //CubeMap 创建空间图
    glGenTextures(1, &envCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        // note that we store each face with 16 bit floating point values
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F,
                     512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // pbr: set up projection and view matrices for capturing data onto the 6 cubemap face directions
    //设置空间图生成参数
   captureProjection.perspective(90.0f, 1.0f, 0.1f, 10.0f);
   lookatMatrix[0].lookAt(QVector3D(0.0f, 0.0f, 0.0f), QVector3D( 1.0f,  0.0f,  0.0f), QVector3D(0.0f, -1.0f,  0.0f));
   lookatMatrix[1].lookAt(QVector3D(0.0f, 0.0f, 0.0f), QVector3D(-1.0f,  0.0f,  0.0f), QVector3D(0.0f, -1.0f,  0.0f));
   lookatMatrix[2].lookAt(QVector3D(0.0f, 0.0f, 0.0f), QVector3D( 0.0f,  1.0f,  0.0f), QVector3D(0.0f,  0.0f,  1.0f));
   lookatMatrix[3].lookAt(QVector3D(0.0f, 0.0f, 0.0f), QVector3D( 0.0f, -1.0f,  0.0f), QVector3D(0.0f,  0.0f, -1.0f));
   lookatMatrix[4].lookAt(QVector3D(0.0f, 0.0f, 0.0f), QVector3D( 0.0f,  0.0f,  1.0f), QVector3D(0.0f, -1.0f,  0.0f));
   lookatMatrix[5].lookAt(QVector3D(0.0f, 0.0f, 0.0f), QVector3D( 0.0f,  0.0f, -1.0f), QVector3D(0.0f, -1.0f,  0.0f));

   // pbr: convert HDR equirectangular environment map to cubemap equivalent
   // 设置椭圆图映射到空间盒的shader，并渲染
    equirectangularToCubemapShader = new QOpenGLShaderProgram();
    equirectangularToCubemapShader->addShaderFromSourceFile(QOpenGLShader::Vertex,":/rectTocube.vert");
    equirectangularToCubemapShader->addShaderFromSourceFile(QOpenGLShader::Fragment,":/rectTocube.frag");
    equirectangularToCubemapShader->link();

    equirectangularToCubemapShader->bind();
    //equirectangularToCubemapShader->setUniformValue("equirectangularMap", 0);
    equirectangularToCubemapShader->setUniformValue("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdrTexture);

    glViewport(0,0,512,512);
    glBindFramebuffer(GL_FRAMEBUFFER,captureFBO);
    for (unsigned int i = 0; i < 6; ++i)
    {
        equirectangularToCubemapShader->setUniformValue("view", lookatMatrix[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        renderCube();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //渲染天空盒的shader
    skyboxShader = new QOpenGLShaderProgram();
    skyboxShader->addShaderFromSourceFile(QOpenGLShader::Vertex,":/skybox.vert");
    skyboxShader->addShaderFromSourceFile(QOpenGLShader::Fragment,":/skybox.frag");
    skyboxShader->link();



    qDebug()<<"initialGL";
}



void GLWidget::showPicture(GLuint ID)
{
    //makeCurrent();
    //glViewport(0,0,width(),height());
    debug_dep->bind();//shader
    debug_dep->setUniformValue("depthMap",0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,ID);

    renderQuad();
    debug_dep->release();
    //doneCurrent();
}



void GLWidget::paintGL()
{
    //显示柱状图
    //glViewport(0, 0, Texwidth, Texheight);
    //glViewport(0,0,width()/2,height());
    //showPicture(hdrTexture);

    equirectangularToCubemapShader->bind();
    equirectangularToCubemapShader->setUniformValue("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdrTexture);
    glViewport(0,0,512,512);
    glBindFramebuffer(GL_FRAMEBUFFER,captureFBO);
    for (unsigned int i = 0; i < 6; ++i)
    {
        equirectangularToCubemapShader->setUniformValue("view", lookatMatrix[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        renderCube();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    glViewport(0, 0, width(), height());
    //glViewport(0,0,512,512);
    //return;
//    glViewport(width()/2,0,width(),height());
//    showPicture(equirectangularMap->textureId());

//    qDebug()<<"draw";
//    glViewport(0, 0, 512, 512); // don't forget to configure the viewport to the capture dimensions.
//    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

//    for (unsigned int i = 0; i < 6; ++i)
//    {
//        equirectangularToCubemapShader->setUniformValue("view", lookatMatrix[i]);
//        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
//        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//        renderCube();
//    }
//    glDepthFunc(GL_LEQUAL);
//    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());


//    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);

    glDepthFunc(GL_LEQUAL);
    skyboxShader->bind();
    skyboxShader->setUniformValue("projection", maincamera.projection);
    skyboxShader->setUniformValue("view",  maincamera.getViewMetrix());
    skyboxShader->setUniformValue("skybox", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP,envCubemap);//envCubemap cubemapTexture

    //renderQuad();
    renderCube();
    glDepthFunc(GL_LESS);
    qDebug()<<"draw";

}

void GLWidget::generateDirShadow()
{
    scene.dirlight->depthMapFBO->bind();
    glClear(GL_COLOR_BUFFER_BIT |GL_DEPTH_BUFFER_BIT);

    simpleDepthShader->bind();//阴影图着色器
    simpleDepthShader->setUniformValue("lightSpaceMatrix",scene.dirlight->getLightMatrix());
    simpleDepthShader->setUniformValue("isPerspective",false);
    for(int i=0;i<scene.objects.size();++i){
        simpleDepthShader->setUniformValue("model",scene.objects.at(i)->model.getmodel());
        scene.objects.at(i)->Draw(*simpleDepthShader);
    }
    scene.dirlight->depthMapFBO->release();

}

void GLWidget::generatePointShadow(int k)
{
    scene.pointlights[k]->depthMapFBO->bind();
    glClear(GL_COLOR_BUFFER_BIT |GL_DEPTH_BUFFER_BIT);
    simpleDepthShader->bind();//阴影图着色器
    simpleDepthShader->setUniformValue("lightSpaceMatrix",scene.pointlights[k]->getLightMatrix());
    simpleDepthShader->setUniformValue("isPerspective",true);
    for(int i=0;i<scene.objects.size();++i){
        simpleDepthShader->setUniformValue("model",scene.objects.at(i)->model.getmodel());
        scene.objects.at(i)->Draw(*simpleDepthShader);
    }
    scene.pointlights[k]->depthMapFBO->release();
}

void GLWidget::resizeGL(int w, int h)
{
    makeCurrent();
    maincamera.projection.setToIdentity();
    maincamera.projection.perspective(45.0f, GLfloat(w) / h, 0.001f, 1000.0f);

//    delete G_Buffer;
//    //G-Buffer
//    //  位置
//    G_Buffer = new QOpenGLFramebufferObject(size(),QOpenGLFramebufferObject::CombinedDepthStencil,GL_TEXTURE_2D,GL_RGBA16F);
//    //  法向量
//    G_Buffer->addColorAttachment(size(),GL_RGBA);
//    //  颜色（HDR）+镜面颜色
//    G_Buffer->addColorAttachment(size(),GL_RGBA16F);
//    //  高光图（只计算光源物体）
//    G_Buffer->addColorAttachment(size(),GL_RGBA16F);
//    //  SSAO
//    G_Buffer->addColorAttachment(size(),GL_RGBA16F);
//    //  viewNormal
//    G_Buffer->addColorAttachment(size(),GL_RGB);
//    //  设置着色器渲染纹理路径
//    G_Buffer->bind();
//    GLenum buffers2[] = { GL_COLOR_ATTACHMENT0,
//                          GL_COLOR_ATTACHMENT1,
//                          GL_COLOR_ATTACHMENT2,
//                          GL_COLOR_ATTACHMENT3,
//                          GL_COLOR_ATTACHMENT4,
//                          GL_COLOR_ATTACHMENT5,};
//    core->glDrawBuffers(6, buffers2);
//    G_Buffer->release();
//    //ssao
//    //qDebug()<<"resizeGL";
//    gaussBlur->resizeGaussBlurFBO(w,h);
//    ssao->resizeSSAO(w,h);
      doneCurrent();
}

void GLWidget::setObjectNumber(int newObjectNumber)
{
    objectNumber = newObjectNumber;
    update();
}

bool GLWidget::getXrotation() const
{
    return xrotation;
}

void GLWidget::setXObjRotationSelected(bool booler)
{
    xrotation = booler;
}

void GLWidget::setYObjRotationSelected(bool booler)
{
    yrotation = booler;
}

void GLWidget::setZObjRotationSelected(bool booler)
{
    zrotation = booler;
}

void GLWidget::setCurrentObjectShader(int index)
{
    if(objectNumber>0){
        scene.shaderPrograms.replace(objectNumber-1,shaderSelector.getShader(index));
    }
    update();
}

void GLWidget::setCurrentObjectEmit(bool emits){
    if(objectNumber>0){
        scene.objects.at(objectNumber-1)->islight = emits;
    }
    update();
}

void GLWidget::setCurrentIndex(int tabIndex)
{
    currentIndex = tabIndex;
}

void GLWidget::setPixObjectNumber(int x, int y)
{
    makeCurrent();
    int yy = height()-y;
    glReadPixels(x,yy,1,1,GL_STENCIL_INDEX,GL_INT,&objectNumber);
    doneCurrent();
    emit objectNumberChanged(objectNumber);

    if(objectNumber>0){objectChangEmitSignal();}
}


void GLWidget::objectChangEmitSignal()
{
    emit objectPosiChanged(scene.objects.at(objectNumber-1)
                           ->model.getPosition());
    emit objectRotationChanged(scene.objects.at(objectNumber-1)
                           ->model.getRotate());
    emit objectScaleChanged(scene.objects.at(objectNumber-1)
                           ->model.getScale());
}

// renderQuad() renders a 1x1 XY quad in NDC
// -----------------------------------------

void GLWidget::renderQuad()
{
    float quadVertices[] = {
        // positions        // texture Coords
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
         1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
         1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    };
    // setup plane VAO
    QOpenGLVertexArrayObject quadVAO;
    QOpenGLBuffer quadVBO(QOpenGLBuffer::VertexBuffer);
    quadVAO.create();
    quadVAO.bind();
    quadVBO.create();
    quadVBO.bind();
    quadVBO.allocate(quadVertices,sizeof(quadVertices));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    quadVAO.release();
}

QOpenGLTexture *GLWidget::loadtexture(QString path)
{
    QImage data(path);
    if(data.isNull()){
        return nullptr;
    }
    QOpenGLTexture* texture = new QOpenGLTexture(data);
    texture->create();
    texture->setWrapMode(QOpenGLTexture::DirectionS,QOpenGLTexture::Repeat);
    texture->setWrapMode(QOpenGLTexture::DirectionT,QOpenGLTexture::Repeat);
    texture->setMinMagFilters(QOpenGLTexture::LinearMipMapLinear,QOpenGLTexture::Linear);
    return texture;
}


unsigned int cubeVAO = 0;
unsigned int cubeVBO = 0;
void GLWidget::renderCube()
{

    // initialize (if necessary)
    if (cubeVAO == 0)
    {
        float vertices[] = {
            // back face
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
             1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
            -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
            // front face
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
             1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
            -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
            // left face
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            // right face
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
             1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left
            // bottom face
            -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
             1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
             1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
             1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
            -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
            // top face
            -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
             1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
             1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right
             1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
            -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
            -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left
        };
        core->glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
        // fill buffer
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // link vertex attributes
        core->glBindVertexArray(cubeVAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        core->glBindVertexArray(0);
    }
    // render Cube
    core->glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    core->glBindVertexArray(0);

}

unsigned int GLWidget::loadCubemap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}
