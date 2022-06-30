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
	glClearColor(0.1, 0.1, 0.1, 1);

    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP,GL_KEEP,GL_REPLACE);

    //shader编译
    for(int i=0;i<shaderSelector.vertexPath.size();++i){
        shaderSelector.compileShader(i);
    }


//--模型加载----------可删除---------------------------
    //Model ourModel("C:/Users/mumu/Desktop/graphics/practicalTraining_2/RenderLite/RenderLite/Picture_source/nanosuit/nanosuit.obj");
    //model = new Model("D:/RenderLite-TMS/RenderLite/RenderLite/Picture_source/iPadScan/2022_06_28_19_38_53/textured_output.obj");
    //model = new Model("D:/RenderLite-TMS/RenderLite/RenderLite/Picture_source/ganyu/ganyu.pmx");
    //model = new Model("D:/RenderLite-TMS/RenderLite/RenderLite/Picture_source/keqing/keqing.pmx");
    //"/Picture_source/paimeng/paimeng.pmx"

    scene.Add(new Model(path+"/Picture_source/keqing/keqing.pmx"));
    scene.shaderProgram.push_back(shaderSelector.getShader(1));
    //-------------------------------------------


    //triangle test
    for(int i=0;i<1;i++){//>5850个三角形面片，报错
        QVector3D v[3];
        v[0] = QVector3D(-10,-i*0.01,-10);
        v[1] = QVector3D(0,-i*0.01,10);
        v[2] = QVector3D(10,-i*0.01,-10);
        QVector3D color(0.2,0.3,0.2);
        Triangle* tri = new Triangle(v,color);
        scene.Add(tri);
        scene.shaderProgram.push_back(shaderSelector.getShader(2));
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
    QMatrix4x4 projection(maincamera.projection);
    QMatrix4x4 m_world;



    //Object Draw
    for(int i=0;i<scene.objects.size();i++){
        glStencilFunc(GL_ALWAYS, i+1, 0xFF);//模板测试始终通过，ref为当前物体编号

        scene.shaderProgram[i]->bind();
        m_world = scene.objects[i]->model.getmodel();
       
        scene.shaderProgram[i]->setUniformValue("model",m_world);
        scene.shaderProgram[i]->setUniformValue("view",maincamera.getViewMetrix());
        scene.shaderProgram[i]->setUniformValue("projection",projection);
        if(scene.shaderProgram[i]==shaderSelector.getShader(1)){
            scene.shaderProgram[i]->setUniformValue("viewPos",maincamera.getCameraPos());
            scene.shaderProgram[i]->setUniformValue("material.shiness",64.0f);
            setDirLight(true,1);
            setPointLight(true,1);
        }

        scene.objects.at(i)->Draw(*scene.shaderProgram[i]);
    }

}

void GLWidget::resizeGL(int w, int h)
{
    maincamera.projection.setToIdentity();
    maincamera.projection.perspective(45.0f, GLfloat(w) / h, 0.001f, 1000.0f);
}

void GLWidget::setObjectNumber(int newObjectNumber)
{

    if (objectNumber == newObjectNumber)return;
    objectNumber = newObjectNumber;
}


void GLWidget::setDirLightActivated(int newDirLightActivated)
{
    dirLightActivated = newDirLightActivated == 0?false:true;
    update();
}

void GLWidget::setPointLightActivated(int newPointLightActivated)
{
    pointLightActivated = newPointLightActivated == 0?false:true;
    update();
}

float GLWidget::getDirSpecular() const
{
    return dirSpecular;
}

void GLWidget::setDirSpecular(float newDirSpecular)
{
    dirSpecular = newDirSpecular;
    update();
}

float GLWidget::getDirDiffuse() const
{
    return dirDiffuse;
}

void GLWidget::setDirDiffuse(float newDirDiffuse)
{
    dirDiffuse = newDirDiffuse;
    update();
}

float GLWidget::getDirAmbient() const
{
    return dirAmbient;
}

void GLWidget::setDirAmbient(float newDirAmbient)
{
    dirAmbient = newDirAmbient;
    update();
}

const QVector3D &GLWidget::getDirLightColor() const
{
    return dirLightColor;
}

void GLWidget::setDirLightColorR(const int r)
{
    dirLightColor.setX(r/255.0f);
    update();
}
void GLWidget::setDirLightColorG(const int g)
{
    dirLightColor.setY(g/255.0f);
    update();
}
void GLWidget::setDirLightColorB(const int b)
{
    dirLightColor.setZ(b/255.0f);
    update();
}

const QVector3D &GLWidget::getDirLightDirection() const
{
    return dirLightDirection;
}

void GLWidget::setDirLightDirectionX(const float x)
{
    dirLightDirection.setX(x);
    update();
}
void GLWidget::setDirLightDirectionY(const float y)
{
    dirLightDirection.setY(y);
    update();
}
void GLWidget::setDirLightDirectionZ(const float z)
{
    dirLightDirection.setZ(z);
    update();
}

float GLWidget::getQuadratic() const
{
    return quadratic;
}

void GLWidget::setQuadratic(float newQuadratic)
{
    quadratic = newQuadratic;
    update();
}

float GLWidget::getLinear() const
{
    return linear;
}

void GLWidget::setLinear(float newLinear)
{
    linear = newLinear;
    update();
}

float GLWidget::getPointSpecular() const
{
    return pointSpecular;
}

void GLWidget::setPointSpecular(float newPointSpecular)
{
    pointSpecular = newPointSpecular;
    update();
}

float GLWidget::getPointDiffuse() const
{
    return pointDiffuse;
}

void GLWidget::setPointDiffuse(float newPointDiffuse)
{
    pointDiffuse = newPointDiffuse;
    update();
}

float GLWidget::getPointAmbient() const
{
    return pointAmbient;
}

void GLWidget::setPointAmbient(float newPointAmbient)
{
    pointAmbient = newPointAmbient;
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

void GLWidget::setCurrentIndex(int tabIndex)
{
    currentIndex = tabIndex;
}

void GLWidget::setPixObjectNumber(int x, int y)
{
    makeCurrent();
    int yy = height()-y;
    glReadPixels(x,yy,1,1,GL_STENCIL_INDEX,GL_INT,&objectNumber);
    qDebug()<<"objectNumber="<<objectNumber;
    doneCurrent();
    emit objectNumberChanged(objectNumber);
}






