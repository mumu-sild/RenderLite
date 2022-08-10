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

#include "Model.h"
#include "glwidget.h"
#include "Global.h"
#include "camera.h"
#include "triangle.h"
#include "Setting.h"
#include "rectangle.h"



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

    glClearColor(backgroundDefaultColor.x(),
                 backgroundDefaultColor.y(),
                 backgroundDefaultColor.z(), 1);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_STENCIL_TEST);

    //帧缓存
    HDRFBO = new QOpenGLFramebufferObject(size(),QOpenGLFramebufferObject::CombinedDepthStencil,GL_TEXTURE_2D,GL_RGBA16F);
    HDRFBO->addColorAttachment(size(),GL_RGBA16F);
    HDRFBO->bind();
    GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    core->glDrawBuffers(2, buffers);
    HDRFBO->release();

    //高斯模糊缓冲
    GBHorizontal = new QOpenGLFramebufferObject(size(),QOpenGLFramebufferObject::NoAttachment,GL_TEXTURE_2D,GL_RGBA16F);
    GBVertical = new QOpenGLFramebufferObject(size(),QOpenGLFramebufferObject::NoAttachment,GL_TEXTURE_2D,GL_RGBA16F);
    //shader编译
    for(int i=0;i<shaderSelector.vertexPath.size();++i){
        shaderSelector.compileShader(i);
    }
    simpleDepthShader = new QOpenGLShaderProgram();
    simpleDepthShader->addShaderFromSourceFile(QOpenGLShader::Vertex,":/simpleDepthShader.vert");
    simpleDepthShader->addShaderFromSourceFile(QOpenGLShader::Fragment,":/simpleDepthShader.frag");
    simpleDepthShader->link();

    debug_dep = new QOpenGLShaderProgram();
    debug_dep->addShaderFromSourceFile(QOpenGLShader::Vertex,":/map_depth.vert");
    debug_dep->addShaderFromSourceFile(QOpenGLShader::Fragment,":/map_depth.frag");
    debug_dep->link();

    BloomShader = new QOpenGLShaderProgram();
    BloomShader->addShaderFromSourceFile(QOpenGLShader::Vertex,":/BloomShader.vert");
    BloomShader->addShaderFromSourceFile(QOpenGLShader::Fragment,":/BloomShader.frag");
    BloomShader->link();

    GaussianBlurShader = new QOpenGLShaderProgram();
    GaussianBlurShader->addShaderFromSourceFile(QOpenGLShader::Vertex,":/map_depth.vert");
    GaussianBlurShader->addShaderFromSourceFile(QOpenGLShader::Fragment,":/GaussianBlur.frag");
    GaussianBlurShader->link();

    //-------光照------------------------------------
    scene.dirlight = new DirLight();
    scene.dirlight->Activated = true;


//--模型加载----------可删除---------------------------
    //Model ourModel("C:/Users/mumu/Desktop/graphics/practicalTraining_2/RenderLite/RenderLite/Picture_source/nanosuit/nanosuit.obj");
    //model = new Model("D:/RenderLite-TMS/RenderLite/RenderLite/Picture_source/iPadScan/2022_06_28_19_38_53/textured_output.obj");
    //model = new Model("D:/RenderLite-TMS/RenderLite/RenderLite/Picture_source/ganyu/ganyu.pmx");
    //model = new Model("D:/RenderLite-TMS/RenderLite/RenderLite/Picture_source/keqing/keqing.pmx");
    //"/Picture_source/paimeng/paimeng.pmx"

    scene.Add(new Model(path+"/Picture_source/keqing/keqing.pmx"));
    scene.objects.at(0)->model.scale(QVector3D(2,2,2));
    //scene.objects.at(0)->model.translate(QVector3D(0,0,0));
    //scene.objects.at(0)->model.rotate(0,180);
    scene.shaderPrograms.push_back(shaderSelector.getShader(1));
    scene.Add(new PointLight(scene.objects.last()->getlightpos(),1));

    scene.Add(new Model("C:/Users/mumu/Desktop/graphics/practicalTraining_2/Picture_source/Tomie/scene/ice.pmx"));
    //scene.objects.at(1)->model.scale(QVector3D(2,2,2));
    //scene.objects.at(1)->model.translate(QVector3D(0,0,0));
    //scene.objects.at(1)->model.rotate(0,180);
    scene.shaderPrograms.push_back(shaderSelector.getShader(1));
    scene.Add(new PointLight(scene.objects.last()->getlightpos(),1));


    //triangle test

    QVector3D v[3];
    v[0] = QVector3D(-10,1,-10);
    v[1] = QVector3D(0,1,10);
    v[2] = QVector3D(10,1,-10);
    QVector3D color(0.8,0.8,0.8);
    Triangle* tri = new Triangle(v,color);
    scene.Add(tri);
    scene.shaderPrograms.push_back(shaderSelector.getShader(2));
    scene.Add(new PointLight(tri->getlightpos(),10));
    tri->islight = true;


    rectangle* rec = new rectangle(300,300);
    rec->model.translate(QVector3D(0,-14,0));
    scene.Add(rec);
    scene.Add(shaderSelector.getShader(3));
    scene.Add(new PointLight(rec->getlightpos(),1));

    qDebug()<<"initialGL";
}



void GLWidget::showShadow(GLuint ID)
{
    //makeCurrent();
    glViewport(0,0,width(),height());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
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
    //第一次层循环，获取光照信息以及shadow map图
    QVector<PointLight*> pointLight;

    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glEnable(GL_DEPTH_TEST);
    glClearColor(1,1,1,1);

    if(scene.dirlight->Activated) generateDirShadow();

    for(int k = 0; k < scene.objects.size(); k++){
        if(scene.objects[k]->islight){
            scene.pointlights[k]->position = scene.objects[k]->getlightpos();
            scene.pointlights[k]->lightNormal = scene.objects.at(k)->getlightNormal();
            scene.pointlights[k]->color = scene.objects.at(k)->color;
            scene.pointlights[k]->width = sqrt(scene.objects.at(k)->getArea());//width越大，半影越大
            generatePointShadow(k);//生成点方向光源阴影图
            pointLight.push_back(scene.pointlights[k]);
        }
    }
    //qDebug()<<"第一层循环完成，生成light属性和light阴影图";

    glClearColor(backgroundDefaultColor.x(),
                 backgroundDefaultColor.y(),
                 backgroundDefaultColor.z(), 1);

//   显示shadow map 图
    if(shadowShow){
        if(objectNumber&&scene.objects.at(objectNumber-1)->islight){
            showShadow(scene.pointlights.at(objectNumber-1)->depthMapFBO->texture());
        }
        else showShadow(scene.dirlight->depthMapFBO->texture());
        return;
    }

//-----场景渲染----------------------------------------------
    HDRFBO->bind();
    glViewport(0, 0, width(), height());
    //Render类，来做渲染
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
    glStencilOp(GL_KEEP,GL_KEEP,GL_REPLACE);
    QMatrix4x4 projection(maincamera.projection);
    QMatrix4x4 m_world;


    //第二层循环设置每个shader的不变参数
    for(int j = 0; j < shaderSelector.fragmentPath.size();j++){
        shaderSelector.getShader(j)->bind();
        shaderSelector.getShader(j)->setUniformValue("view",maincamera.getViewMetrix());
        shaderSelector.getShader(j)->setUniformValue("projection",projection);

        if(j==shaderTypes::SHADER_LIGHT||j==shaderTypes::SHADER_LIGHTCOLOR){//1,3
            shaderSelector.getShader(j)->setUniformValue("viewPos",maincamera.getCameraPos());
            shaderSelector.getShader(j)->setUniformValue("material.shiness",64.0f);
            shaderSelector.setLightDir(j,scene.dirlight);
            shaderSelector.setPointDir(j,pointLight);
            //调试参数
            shaderSelector.getShader(j)->setUniformValue("gamma",gamma);
            shaderSelector.getShader(j)->setUniformValue("blinn",blinn);
            shaderSelector.getShader(j)->setUniformValue("toneMapping",toneMapping);
        }
    }


    //Object Draw
    for(int i=0;i<scene.objects.size();i++){
        glStencilFunc(GL_ALWAYS, i+1, 0xFF);//模板测试始终通过，ref为当前物体编号

        scene.shaderPrograms[i]->bind();
        m_world = scene.objects[i]->model.getmodel();
        scene.shaderPrograms[i]->setUniformValue("model",m_world);
        if(scene.shaderPrograms[i] == shaderSelector.getShader(shaderTypes::SHADER_COLOR)){//2
            if(scene.objects.at(i)->islight){
                scene.shaderPrograms[i]->setUniformValue("color",scene.pointlights.at(i)->color*scene.pointlights.at(i)->intensity);
            }
            else scene.shaderPrograms[i]->setUniformValue("color",scene.objects.at(i)->color);
        }
        if(scene.shaderPrograms[i] == shaderSelector.getShader(shaderTypes::SHADER_LIGHTCOLOR)){//3
           scene.shaderPrograms[i]->setUniformValue("material.color",scene.objects[i]->color);
        }
        scene.objects.at(i)->Draw(*scene.shaderPrograms[i]);
    }


    //边框
    if(objectNumber){
        glDisable(GL_CULL_FACE);
        glStencilFunc(GL_NOTEQUAL,objectNumber,0xFF);
        glStencilOp(GL_KEEP,GL_KEEP,GL_REPLACE);

        shaderSelector.getShader(2)->bind();
        m_world = scene.objects[objectNumber-1]->model.getmodel();
        m_world.scale(frameScale);
        shaderSelector.getShader(2)->setUniformValue("model",m_world);
        shaderSelector.getShader(2)->setUniformValue("color",QVector3D(1,1,1));
        scene.objects.at(objectNumber-1)->Draw(*shaderSelector.getShader(2));
        glEnable(GL_CULL_FACE);
    }

    HDRFBO->release();

    //高斯模糊
    GLboolean horizontal = true, first_iteration = true;
    GLuint amount = 10;
    GaussianBlurShader->bind();
    for (GLuint i = 0; i < amount; i++)
    {
        if(horizontal)GBHorizontal->bind();
        else GBVertical->bind();
        //glClear(GL_COLOR_BUFFER_BIT);
        GaussianBlurShader->setUniformValue("horizontal",horizontal);
        GaussianBlurShader->setUniformValue("image",0);
        glActiveTexture(GL_TEXTURE0);
        unsigned int id = 0;
        if(first_iteration){
            id = HDRFBO->textures().at(1);
        }
        else{
            if(horizontal)id = GBVertical->texture();
            else id = GBHorizontal->texture();
        }
        glBindTexture(GL_TEXTURE_2D, id);
        renderQuad();
        horizontal = !horizontal;
        if (first_iteration)
            first_iteration = false;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //HDR输出
    glViewport(0,0,width(),height());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    BloomShader->bind();//shader

    BloomShader->setUniformValue("RenderResult",0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,HDRFBO->textures().at(0));

    BloomShader->setUniformValue("bloomBlur",1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D,GBVertical->texture());

    if(HDRNUM==0){
        BloomShader->setUniformValue("addBloom",false);
    }
    else BloomShader->setUniformValue("addBloom",true);

    renderQuad();
    QOpenGLFramebufferObject::blitFramebuffer(nullptr,HDRFBO,GL_STENCIL_BUFFER_BIT);

    BloomShader->release();

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


    delete HDRFBO;
    HDRFBO = new QOpenGLFramebufferObject(w,h,QOpenGLFramebufferObject::CombinedDepthStencil,GL_TEXTURE_2D,GL_RGBA16F);
    HDRFBO->addColorAttachment(w,h,GL_RGBA16F);
    HDRFBO->bind();
    GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    core->glDrawBuffers(2, buffers);
    HDRFBO->release();

    //高斯模糊缓冲
    delete GBHorizontal;
    delete GBVertical;
    GBHorizontal = new QOpenGLFramebufferObject(w,h,QOpenGLFramebufferObject::NoAttachment,GL_TEXTURE_2D,GL_RGBA16F);
    GBVertical = new QOpenGLFramebufferObject(w,h,QOpenGLFramebufferObject::NoAttachment,GL_TEXTURE_2D,GL_RGBA16F);

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


