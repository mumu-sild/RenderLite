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




bool GLWidget::m_transparent = false;

GLWidget::GLWidget(QWidget *parent)
    : QOpenGLWidget(parent)
{
    m_core = QSurfaceFormat::defaultFormat().profile() == QSurfaceFormat::CoreProfile;
    // --transparent causes the clear color to be transparent. Therefore, on systems that
    // support it, the widget will become transparent apart from the logo.
    if (m_transparent) {
        QSurfaceFormat fmt = format();
        fmt.setAlphaBufferSize(8);
        setFormat(fmt);
    }
    this->setFocusPolicy(Qt::StrongFocus);
}

GLWidget::~GLWidget()
{
    cleanup();
}

QSize GLWidget::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize GLWidget::sizeHint() const
{
    return QSize(400, 400);
}

static void qNormalizeAngle(int &angle)
{
    while (angle < 0)
        angle += 360 * 16;
    while (angle > 360 * 16)
        angle -= 360 * 16;
}

void GLWidget::setXRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != m_xRot) {
        m_xRot = angle;
        emit xRotationChanged(angle);
        update();
    }
}

void GLWidget::setYRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != m_yRot) {
        m_yRot = angle;
        emit yRotationChanged(angle);
        update();
    }
}

void GLWidget::setZRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != m_zRot) {
        m_zRot = angle;
        emit zRotationChanged(angle);
        update();
    }
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
    glClearColor(0.1, 0.1, 0.1, m_transparent ? 0 : 1);


    shaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,":/lightObject.vsh");
    shaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment,":/lightObject.fsh");
    shaderProgram.link();
    qDebug()<<"log:"<<shaderProgram.log();

    shaderProgram.bind();

//--将VBO，VAO,VEO作为一个整体，存储数据结构----------------------------------------
    //Model ourModel("C:/Users/mumu/Desktop/graphics/practicalTraining_2/RenderLite/RenderLite/Picture_source/nanosuit/nanosuit.obj");
    //model = new Model("D:/RenderLite-TMS/RenderLite/RenderLite/Picture_source/iPadScan/2022_06_28_19_38_53/textured_output.obj");
    //model = new Model("D:/RenderLite-TMS/RenderLite/RenderLite/Picture_source/ganyu/ganyu.pmx");
    //model = new Model("D:/RenderLite-TMS/RenderLite/RenderLite/Picture_source/keqing/keqing.pmx");
    //"/Picture_source/paimeng/paimeng.pmx"
    Model* model = new Model(path+"/Picture_source/keqing/keqing.pmx");
    scene.Add(model);
    //model = new Model(path+"/Picture_source/ganyu/ganyu.pmx");
    //scene.Add(model);
    //triangle test
    for(int i=0;i<1;i++){//>5850个三角形面片，报错
        QVector3D v[3];
        v[0] = QVector3D(-10,-i*0.01,-10);
        v[1] = QVector3D(0,-i*0.01,10);
        v[2] = QVector3D(10,-i*0.01,-10);
        QVector3D color(0.2,0.3,0.2);
        Triangle* tri = new Triangle(v,color);
        scene.Add(tri);
    }

//----摄像机类------------------------------------------------------------
    // Our camera never changes in this example.

    m_camera.setToIdentity();
    m_camera.translate(0, 0, -1);

    //-----------------------光源位置/方向/强度---------------------
    pointLightPosition = new QVector3D[numPointLight]{
                QVector3D(0.7f,  0.2f,  2.0f),
                QVector3D(2.3f, -3.3f, -4.0f),
                QVector3D(-4.0f,  2.0f, -12.0f),
                QVector3D(0.0f,  0.0f, -3.0f)
    };
    pointLightAmbient = QVector3D(0.05f,0.05f,0.05f);
    pointLightDiffuse = QVector3D(0.8f, 0.8f, 0.8f);
    pointLightSpecular = QVector3D(1.0f,1.0f,1.0f);
    dirLightDirection  = QVector3D(-0.2f, -1.0f, -0.3f);
    dirLightAmbient = QVector3D(0.4f, 0.4f, 0.4f);
    dirLightDiffuse = QVector3D(0.4f, 0.4f, 0.4f);
    dirLightSpecular = QVector3D(0.5f, 0.5f, 0.5f);
    constant = 1.0f;
    linear = 0.09f;
    quadratic = 0.032f;
    lightData = new LightData(&shaderProgram);

}

void GLWidget::paintGL()
{
    //Render类，来做渲染
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);



//    m_world.setToIdentity();
//    m_world.rotate(180.0f - (m_xRot / 16.0f), 1, 0, 0);
//    m_world.rotate(m_yRot / 16.0f, 0, 1, 0);
//    m_world.rotate(m_zRot / 16.0f, 0, 0, 1);
    //m_world.scale(0.3,0.3,0.3);



    shaderProgram.bind();
    shaderProgram.setUniformValue("viewPos",maincamera.getCameraPos());
    shaderProgram.setUniformValue("material.shiness",64.0f);


    shaderProgram.setUniformValue("projection",m_proj);
    shaderProgram.setUniformValue("view",maincamera.getViewMetrix());
    shaderProgram.setUniformValue("model",m_world);

    lightData->activateDirLight();
    lightData->setDirLightDirection(dirLightDirection);
    lightData->setDirLightAmbient(dirLightAmbient);
    lightData->setDirLightDiffuse(dirLightDiffuse);
    lightData->setDirLightSpecular(dirLightSpecular);

    lightData->activatePointLight();
    lightData->setPointLightPosition(pointLightPosition);
    lightData->setPointLightAmbient(pointLightAmbient);
    lightData->setPointLightDiffuse(pointLightDiffuse);
    lightData->setPointLightSpecular(pointLightSpecular);
    lightData->setConstant(constant);
    lightData->setLinear(linear);
    lightData->setQuadratic(quadratic);

    qDebug()<<"scene.size:"<<scene.objects.size();
    //Object Draw
    for(int i=0;i<scene.objects.size();i++){
        m_world = scene.objects[i]->model.getmodel();
        shaderProgram.setUniformValue("model",m_world);
        qDebug()<<"DRAW:"<<i;
        scene.objects.at(i)->Draw(shaderProgram);
        //qDebug()<<"Draw Finish"<<i;
    }
    //model->Draw(shaderProgram);
    //model1->Draw(shaderProgram);


}

void GLWidget::resizeGL(int w, int h)
{
    m_proj.setToIdentity();
    m_proj.perspective(45.0f, GLfloat(w) / h, 0.001f, 1000.0f);
    qDebug()<<"resizeGL";
}

void GLWidget::importModel(QString modelPath)
{
    scene.Add(new Model(modelPath));
    update();
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

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    int dx = m_lastPos.x() - event->x();
    int dy = m_lastPos.y() - event->y();

//    if (event->buttons() & Qt::LeftButton) {
//        setXRotation(m_xRot + 8 * dy);
//        setYRotation(m_yRot + 8 * dx);
//    } else if (event->buttons() & Qt::RightButton) {
//        setXRotation(m_xRot + 8 * dy);
//        setZRotation(m_zRot + 8 * dx);
//    }
    if(QApplication::keyboardModifiers()==Qt::AltModifier
            &&event->buttons() == Qt::LeftButton)
    {
        maincamera.rotateCamera(dx,dy);
        update();
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
