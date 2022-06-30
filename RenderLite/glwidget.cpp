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
#include <QCoreApplication>
#include <math.h>
#include <QDir>
#include <QString>
#include <iostream>
#include <QDebug>

#include "Model.h"
#include "glwidget.h"
#include "Global.h"
#include "camera.h"
#include "lightData.h"
#include "triangle.h"
#include "Setting.h"



GLWidget::GLWidget(QWidget *parent)
    : QOpenGLWidget(parent)
{
    m_core = QSurfaceFormat::defaultFormat().profile() == QSurfaceFormat::CoreProfile;
    this->setFocusPolicy(Qt::StrongFocus);
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
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP,GL_KEEP,GL_REPLACE);

    //shaderSelector = ShaderSelector();


//--模型导入----------------------------------------
    //Model ourModel("C:/Users/mumu/Desktop/graphics/practicalTraining_2/RenderLite/RenderLite/Picture_source/nanosuit/nanosuit.obj");
    //model = new Model("D:/RenderLite-TMS/RenderLite/RenderLite/Picture_source/iPadScan/2022_06_28_19_38_53/textured_output.obj");
    //model = new Model("D:/RenderLite-TMS/RenderLite/RenderLite/Picture_source/ganyu/ganyu.pmx");
    //model = new Model("D:/RenderLite-TMS/RenderLite/RenderLite/Picture_source/keqing/keqing.pmx");
    //"/Picture_source/paimeng/paimeng.pmx"
    Model* model = new Model(path+"/Picture_source/keqing/keqing.pmx");
    scene.Add(model);

//TODO：获取当前模型渲染的shader类型-------------
    shaderSelector.compileShader(1);
    //-------------------------------------------
    scene.shaderProgram.push_back(shaderSelector.getShader(0));
    //triangle test
    for(int i=0;i<1;i++){//>5850个三角形面片，报错
        QVector3D v[3];
        v[0] = QVector3D(-10,-i*0.01,-10);
        v[1] = QVector3D(0,-i*0.01,10);
        v[2] = QVector3D(10,-i*0.01,-10);
        QVector3D color(0.2,0.3,0.2);
        Triangle* tri = new Triangle(v,color);
        scene.Add(tri);
        //TODO：获取当前模型渲染的shader类型-------------
        shaderSelector.compileShader();
        //-------------------------------------------
        scene.shaderProgram.push_back(shaderSelector.getShader(i+1));
    }
//-----------------光源位置/方向/强度---------------------
    pointLightPosition.push_back(QVector3D(0.7f,  0.2f,  2.0f));
    pointLightPosition.push_back(QVector3D(2.3f, -3.3f, -4.0f));
    pointLightPosition.push_back(QVector3D(-4.0f,  2.0f, -12.0f));
    pointLightPosition.push_back(QVector3D(0.0f,  0.0f, -3.0f));
    pointAmbient = 0.05f;
    pointDiffuse = 0.8f;
    pointSpecular = 1.0f;
    pointLightColor.push_back(QVector3D(1.0f,1.0f,1.0f));
    pointLightColor.push_back(QVector3D(1.0f,1.0f,1.0f));
    pointLightColor.push_back(QVector3D(1.0f,1.0f,1.0f));
    pointLightColor.push_back(QVector3D(1.0f,1.0f,1.0f));
    constant = 1.0f;
    linear = 0.09f;
    quadratic = 0.032f;

    dirLightDirection = QVector3D(-0.2f, -1.0f, -0.3f);
    dirLightColor = QVector3D(1.0f,1.0f,1.0f);
    dirAmbient = 0.4f;
    dirDiffuse = 0.5f;
    dirSpecular = 0.5f;

}

void GLWidget::paintGL()
{
    //Render类，来做渲染
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);

    shaderSelector.getShader(1)->bind();
//    shaderSelector.getShader()->setUniformValue("viewPos",maincamera.getCameraPos());
//    shaderSelector.getShader()->setUniformValue("material.shiness",64.0f);


    shaderSelector.getShader(1)->setUniformValue("projection",m_proj);
    shaderSelector.getShader(1)->setUniformValue("view",maincamera.getViewMetrix());
    shaderSelector.getShader(1)->setUniformValue("model",m_world);


    shaderSelector.getShader(0)->bind();
    shaderSelector.getShader(0)->setUniformValue("viewPos",maincamera.getCameraPos());
    shaderSelector.getShader(0)->setUniformValue("material.shiness",64.0f);


    shaderSelector.getShader(0)->setUniformValue("projection",m_proj);
    shaderSelector.getShader(0)->setUniformValue("view",maincamera.getViewMetrix());
    shaderSelector.getShader(0)->setUniformValue("model",m_world);
    setDirLight(true,0);
    setPointLight(true,0);


    //Object Draw
    for(int i=0;i<scene.objects.size();i++){
        glStencilFunc(GL_ALWAYS, i+1, 0xFF);//模板测试始终通过，ref为当前物体编号

        m_world = scene.objects[i]->model.getmodel();
        shaderSelector.getShader(i)->setUniformValue("model",m_world);
        scene.shaderProgram[i]->bind();
        scene.objects.at(i)->Draw(*scene.shaderProgram[i]);
    }
}

void GLWidget::resizeGL(int w, int h)
{
    m_proj.setToIdentity();
    m_proj.perspective(45.0f, GLfloat(w) / h, 0.001f, 1000.0f);
}

void GLWidget::importModel(QString modelPath)
{
    makeCurrent();
    scene.Add(new Model(modelPath));
    update();
    doneCurrent();
}


void GLWidget::setDirLight(bool activate, int objNum)
{
    lightData = new LightData(shaderSelector.getShader(objNum));
    if(!activate){
        lightData->disableDirLight();
        return;
    }  
        lightData->activateDirLight();
        lightData->setDirLightDirection(dirLightDirection);
        lightData->setDirLightAmbientColor(dirLightColor,dirAmbient);
        lightData->setDirLightDiffuseColor(dirLightColor,dirDiffuse);
        lightData->setDirLightSpecularColor(dirLightColor,dirSpecular);
        delete lightData;
        return;
}

void GLWidget::setPointLight(bool activate, int objNum)
{
    lightData = new LightData(shaderSelector.getShader(objNum));
    if(!activate){
        lightData->disablepointLight();
        return;
    }
    lightData->activatePointLight();
    lightData->setPointLightPosition(pointLightPosition);
    lightData->setPointLightAmbientColor(pointLightColor,pointAmbient);
    lightData->setPointLightDiffuseColor(pointLightColor,pointDiffuse);
    lightData->setPointLightSpecularColor(pointLightColor,pointSpecular);
    lightData->setConstant(constant);
    lightData->setLinear(linear);
    lightData->setQuadratic(quadratic);
    delete lightData;
    return;
}


void GLWidget::mousePressEvent(QMouseEvent *event)
{
    m_lastPos = event->pos();
    //鼠标左键按下事件
    if(event->button()==Qt::LeftButton)
    {
        qDebug()<<"左键按下"<<event->pos();
    }
    //鼠标右键按下事件
    if(event->button()==Qt::RightButton)
    {
        qDebug()<<"右键按下";
    }
    //鼠标中键按下事件
    if(event->button()==Qt::MidButton)
    {
        qDebug()<<"滚轮按下了";
    }
    //鼠标左键按下和按下alt键
    if(QApplication::keyboardModifiers()==Qt::AltModifier
            &&event->button() == Qt::LeftButton)
    {
        qDebug()<<"Alt键和鼠标左键按下";
        m_camera.translate(0, 0.2, 0);
        update();
    }
    //鼠标右键按下和按下alt键
    if(QApplication::keyboardModifiers()==Qt::AltModifier
            &&event->button() == Qt::RightButton)
    {
        qDebug()<<"Alt键和鼠标右键按下";
        m_camera.translate(0, -0.2, 0);
        update();
    }
}

int GLWidget::getPixObjectNumber(int x, int y)
{
    makeCurrent();
    int number;
    this->glReadPixels(width()/2,height()/2,1,1,GL_STENCIL_INDEX,GL_INT,&number);
    return number;
    doneCurrent();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    int dx = m_lastPos.x() - event->x();
    int dy = m_lastPos.y() - event->y();

    //摄像机旋转
    if(currentIndex==1)
    {
        if(QApplication::keyboardModifiers()==Qt::AltModifier
                &&event->buttons() == Qt::LeftButton)
        {
            maincamera.rotateCamera(dx,dy);
            update();
        }
    }

    //物体旋转
    if(currentIndex==0)
    {
        if(xrotation&&QApplication::keyboardModifiers()==Qt::AltModifier
                &&event->buttons() == Qt::LeftButton)
        {
            qDebug()<<"物体的x轴旋转";
            scene.objects.at(0)->model.rotate(0,dx*0.5);
            update();
        }
        if(yrotation&&QApplication::keyboardModifiers()==Qt::AltModifier
                &&event->buttons() == Qt::LeftButton)
        {
            qDebug()<<"物体的y轴旋转";
            scene.objects.at(0)->model.rotate(1,dx*0.5);
            update();
        }
        if(zrotation&&QApplication::keyboardModifiers()==Qt::AltModifier
                &&event->buttons() == Qt::LeftButton)
        {
            qDebug()<<"物体的z轴旋转";
            scene.objects.at(0)->model.rotate(2,dx*0.5);
            update();
        }
    }

    m_lastPos = event->pos();
}

//鼠标滚轮滚动事件
void GLWidget::wheelEvent(QWheelEvent *event)
{
    if(event->delta()>0)
    {
        qDebug()<<"滚轮往前滚";
        maincamera.moveForBackward(-1);

        emit xCameraPosiChanged(maincamera.getCameraPos().x());
        emit yCameraPosiChanged(maincamera.getCameraPos().y());
        emit zCameraPosiChanged(maincamera.getCameraPos().z());

        emit xCameraFocusChanged(maincamera.getCameraFocus().x());
        emit yCameraFocusChanged(maincamera.getCameraFocus().y());
        emit zCameraFocusChanged(maincamera.getCameraFocus().z());

        qDebug()<<maincamera.getCameraPos().x();
        qDebug()<<maincamera.getCameraPos().y();
        qDebug()<<maincamera.getCameraPos().z();

        update();
    }
    if(event->delta()<0)
    {
        qDebug()<<"滚轮往后滚";
        maincamera.moveForBackward(1);

        emit xCameraPosiChanged(maincamera.getCameraPos().x());
        emit yCameraPosiChanged(maincamera.getCameraPos().y());
        emit zCameraPosiChanged(maincamera.getCameraPos().z());

        emit xCameraFocusChanged(maincamera.getCameraFocus().x());
        emit yCameraFocusChanged(maincamera.getCameraFocus().y());
        emit zCameraFocusChanged(maincamera.getCameraFocus().z());

        qDebug()<<maincamera.getCameraPos().x();
        qDebug()<<maincamera.getCameraPos().y();
        qDebug()<<maincamera.getCameraPos().z();

        update();
    }
}

//键盘按下事件
void GLWidget::keyPressEvent(QKeyEvent *event)
{
    if(event->key()==Qt::Key_W)
    {
        maincamera.moveUpDownward(1);

        emit xCameraPosiChanged(maincamera.getCameraPos().x());
        emit yCameraPosiChanged(maincamera.getCameraPos().y());
        emit zCameraPosiChanged(maincamera.getCameraPos().z());

        emit xCameraFocusChanged(maincamera.getCameraFocus().x());
        emit yCameraFocusChanged(maincamera.getCameraFocus().y());
        emit zCameraFocusChanged(maincamera.getCameraFocus().z());

        qDebug()<<maincamera.getCameraPos().x();
        qDebug()<<maincamera.getCameraPos().y();
        qDebug()<<maincamera.getCameraPos().z();

        update();
    }
    if(event->key()==Qt::Key_S)
    {
        qDebug()<<"按下S键";
        maincamera.moveUpDownward(-1);

        emit xCameraPosiChanged(maincamera.getCameraPos().x());
        emit yCameraPosiChanged(maincamera.getCameraPos().y());
        emit zCameraPosiChanged(maincamera.getCameraPos().z());

        emit xCameraFocusChanged(maincamera.getCameraFocus().x());
        emit yCameraFocusChanged(maincamera.getCameraFocus().y());
        emit zCameraFocusChanged(maincamera.getCameraFocus().z());

        qDebug()<<maincamera.getCameraPos().x();
        qDebug()<<maincamera.getCameraPos().y();
        qDebug()<<maincamera.getCameraPos().z();

        update();
    }
    if(event->key()==Qt::Key_A)
    {
        maincamera.moveLRward(-1);

        emit xCameraPosiChanged(maincamera.getCameraPos().x());
        emit yCameraPosiChanged(maincamera.getCameraPos().y());
        emit zCameraPosiChanged(maincamera.getCameraPos().z());

        emit xCameraFocusChanged(maincamera.getCameraFocus().x());
        emit yCameraFocusChanged(maincamera.getCameraFocus().y());
        emit zCameraFocusChanged(maincamera.getCameraFocus().z());

        qDebug()<<maincamera.getCameraPos().x();
        qDebug()<<maincamera.getCameraPos().y();
        qDebug()<<maincamera.getCameraPos().z();

        update();
    }
    if(event->key()==Qt::Key_D)
    {
        maincamera.moveLRward(1);

        emit xCameraPosiChanged(maincamera.getCameraPos().x());
        emit yCameraPosiChanged(maincamera.getCameraPos().y());
        emit zCameraPosiChanged(maincamera.getCameraPos().z());

        emit xCameraFocusChanged(maincamera.getCameraFocus().x());
        emit yCameraFocusChanged(maincamera.getCameraFocus().y());
        emit zCameraFocusChanged(maincamera.getCameraFocus().z());

        qDebug()<<maincamera.getCameraPos().x();
        qDebug()<<maincamera.getCameraPos().y();
        qDebug()<<maincamera.getCameraPos().z();

        update();
    }
}

//键盘松开事件
void GLWidget::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key()==Qt::Key_W)
    {
    }
    if(event->key()==Qt::Key_S)
    {
    }
    if(event->key()==Qt::Key_A)
    {
    }
    if(event->key()==Qt::Key_D)
    {
    }
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

void GLWidget::setCurrentIndex(int tabIndex)
{
    currentIndex = tabIndex;
}

void GLWidget::setXCameraPosi(double meters)
{
    maincamera.setXCameraPosi(meters);
    update();
}

void GLWidget::setYCameraPosi(double meters)
{
    maincamera.setYCameraPosi(meters);
    update();
}

void GLWidget::setZCameraPosi(double meters)
{
    maincamera.setZCameraPosi(meters);
    update();
}

void GLWidget::setXCameraFocus(double meters)
{
    maincamera.setXCameraFocus(meters);
    update();
}

void GLWidget::setYCameraFocus(double meters)
{
    maincamera.setYCameraFocus(meters);
    update();
}

void GLWidget::setZCameraFocus(double meters)
{
    maincamera.setZCameraFocus(meters);
    update();
}


