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
//#include "lightData.h"
#include "triangle.h"
#include "Setting.h"
#include "lightData.h"
#include "rectangle.h"


float PointLight::ambient = pointLightDefaultAmbient;
float PointLight::diffuse = PointLightDefaultDiffuse;
float PointLight::specular = PointLightDefaultSpecular;
float PointLight::constant = PointLightDefaultConstant;
float PointLight::linear = PointLightDefaultLinear;
float PointLight::quadratic=PointLightDefaultQuadratic;
float DirLight::ambient = DirLightDefaultAmbient;
float DirLight::diffuse = DirLightDefaultDiffuse;
float DirLight::specular = DirLightDefaultSpecular;

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
    glClearColor(backgroundDefaultColor.x(),
                 backgroundDefaultColor.y(),
                 backgroundDefaultColor.z(), 1);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_STENCIL_TEST);

    //shader编译
    for(int i=0;i<shaderSelector.vertexPath.size();++i){
        shaderSelector.compileShader(i);
    }

    //-------光照------------------------------------
    scene.dirlight = new DirLight();
    scene.dirlight->dirLightActivated = true;


//--模型加载----------可删除---------------------------
    //Model ourModel("C:/Users/mumu/Desktop/graphics/practicalTraining_2/RenderLite/RenderLite/Picture_source/nanosuit/nanosuit.obj");
    //model = new Model("D:/RenderLite-TMS/RenderLite/RenderLite/Picture_source/iPadScan/2022_06_28_19_38_53/textured_output.obj");
    //model = new Model("D:/RenderLite-TMS/RenderLite/RenderLite/Picture_source/ganyu/ganyu.pmx");
    //model = new Model("D:/RenderLite-TMS/RenderLite/RenderLite/Picture_source/keqing/keqing.pmx");
    //"/Picture_source/paimeng/paimeng.pmx"

    scene.Add(new Model(path+"/Picture_source/scanmodel/model/textured_output.obj"));
    scene.objects.at(0)->model.scale(QVector3D(80,80,80));
    scene.objects.at(0)->model.translate(QVector3D(0,15,0));
    scene.shaderPrograms.push_back(shaderSelector.getShader(1));
    scene.Add(new PointLight(scene.objects.last()->getlightpos(),QVector3D(1,1,1)));


    //triangle test

    QVector3D v[3];
    v[0] = QVector3D(-10,1,-10);
    v[1] = QVector3D(0,1,10);
    v[2] = QVector3D(10,1,-10);
    QVector3D color(0.2,0.3,0.2);
    Triangle* tri = new Triangle(v,color);
    scene.Add(tri);
    scene.shaderPrograms.push_back(shaderSelector.getShader(2));
    scene.Add(new PointLight(tri->getlightpos(),QVector3D(1,1,1)));
    tri->islight = true;


    rectangle* rec = new rectangle(300,300);
    scene.Add(rec);
    scene.Add(shaderSelector.getShader(3));
    scene.Add(new PointLight(rec->getlightpos(),QVector3D(1,1,1)));

}

void GLWidget::paintGL()
{
    //Render类，来做渲染
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
    glStencilOp(GL_KEEP,GL_KEEP,GL_REPLACE);
    QMatrix4x4 projection(maincamera.projection);
    QMatrix4x4 m_world;
    QVector<PointLight*> pointLight;

    for(int k = 0; k < scene.objects.size(); k++){
            if(scene.objects[k]->islight){
                //qDebug()<<"pointLight"<<k;
                scene.pointlights[k]->setPosition(scene.objects[k]->getlightpos());
                scene.pointlights[k]->lightNormal = scene.objects.at(k)->getlightNormal();
                scene.pointlights[k]->color = scene.objects.at(k)->color;
                pointLight.push_back(scene.pointlights[k]);
            }
        }

    for(int j = 0; j < shaderSelector.fragmentPath.size();j++){
        //qDebug()<<j<<"shaderSelector";
        shaderSelector.getShader(j)->bind();
        shaderSelector.getShader(j)->setUniformValue("view",maincamera.getViewMetrix());
        shaderSelector.getShader(j)->setUniformValue("projection",projection);

        if(j==shaderTypes::SHADER_LIGHT||j==shaderTypes::SHADER_LIGHTCOLOR){
            shaderSelector.getShader(j)->setUniformValue("viewPos",maincamera.getCameraPos());
            shaderSelector.getShader(j)->setUniformValue("material.shiness",64.0f);
            shaderSelector.setLightDir(j,scene.dirlight);
            shaderSelector.setPointDir(j,pointLight);
        }
    }


    //Object Draw
    for(int i=0;i<scene.objects.size();i++){
        glStencilFunc(GL_ALWAYS, i+1, 0xFF);//模板测试始终通过，ref为当前物体编号

        //qDebug()<<i;
        scene.shaderPrograms[i]->bind();
        m_world = scene.objects[i]->model.getmodel();
        scene.shaderPrograms[i]->setUniformValue("model",m_world);
        if(scene.shaderPrograms[i] == shaderSelector.getShader(shaderTypes::SHADER_COLOR)){
            scene.shaderPrograms[i]->setUniformValue("color",scene.objects.at(i)->color);
        }
        if(scene.shaderPrograms[i] == shaderSelector.getShader(shaderTypes::SHADER_LIGHTCOLOR)){
           scene.shaderPrograms[i]->setUniformValue("material.color",scene.objects[i]->color);
       }
        scene.objects.at(i)->Draw(*scene.shaderPrograms[i]);
        //qDebug()<<i<< "Draw";
    }


    //边框
    if(objectNumber){
        glDisable(GL_CULL_FACE);
        //qDebug()<<"objectnumber:"<<objectNumber;
//        glDisable(GL_DEPTH_TEST);
        glStencilFunc(GL_NOTEQUAL,objectNumber,0xFF);
        glStencilOp(GL_KEEP,GL_KEEP,GL_REPLACE);

        shaderSelector.getShader(2)->bind();
        m_world = scene.objects[objectNumber-1]->model.getmodel();
        m_world.scale(frameScale);
        shaderSelector.getShader(2)->setUniformValue("model",m_world);
        shaderSelector.getShader(2)->setUniformValue("color",QVector3D(1,1,1));
        scene.objects.at(objectNumber-1)->Draw(*shaderSelector.getShader(2));
        glEnable(GL_CULL_FACE);
//        glEnable(GL_DEPTH_TEST);
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
//        if(emits)scene.shaderPrograms.replace(objectNumber-1,shaderSelector.getShader(shaderTypes::SHADER_COLOR));
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
//    qDebug()<<"objectNumber="<<objectNumber;
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



