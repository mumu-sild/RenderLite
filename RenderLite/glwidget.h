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

#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QMatrix4x4>


#include "Model.h"//ģ��������
#include "camera.h"
#include "lightData.h"
#include "scene.h"
#include "shaderSelector.h"

extern QString path;
#define numPointLight 4

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram)

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

private:
    //
    Scene scene;
    ShaderSelector shaderSelector;
    camera maincamera;

    //��������
    QPoint m_lastPos;
    bool xrotation = true;
    bool yrotation = false;
    bool zrotation = false;
    int currentIndex = 1;
    int objectNumber = 0;
    int pixX=0,pixY=0;

    enum shaderTypes{
        SHADER_TEXTURE = 0,
        SHADER_LIGHT = 1,
        SHADER_COLOR = 2
    };


//Ӧ�õ�����һ����----------------------------------------------------------
    QVector<QVector3D> pointLightPosition;
    QVector<QVector3D> pointLightColor;
//    QVector3D pointLightAmbientColor;
//    QVector3D pointLightDiffuseColor;
//    QVector3D pointLightSpecularColor;
    float pointAmbient;
    float pointDiffuse;
    float pointSpecular;
    float constant;
    float linear;
    float quadratic;

    QVector3D dirLightDirection;
    QVector3D dirLightColor;
//    QVector3D dirLightAmbientColor;
//    QVector3D dirLightDiffuseColor;
//    QVector3D dirLightSpecularColor;

    float dirAmbient;
    float dirDiffuse;
    float dirSpecular;
    LightData* lightData;
//----------------------------------------------



public:
    GLWidget(QWidget *parent = nullptr);
    ~GLWidget();

    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
public:
    //����ģ��
    void importModel(QString modelPath);
    bool getXrotation() const;

    void setXObjRotationSelected(bool booler);
    void setYObjRotationSelected(bool booler);
    void setZObjRotationSelected(bool booler);


    //shader��������
    void setDirLight(bool activate,int objNum);
    void setPointLight(bool activate,int objNum);


    void setCurrentIndex(int tabIndex);
    void setPixObjectNumber(int x,int y);
    int getObjectSize();
    void setObjectNumber(int newObjectNumber);

public slots:
    void cleanup();
    void setXCameraPosi(double meters);
    void setYCameraPosi(double meters);
    void setZCameraPosi(double meters);

    void setXCameraFocus(double meters);
    void setYCameraFocus(double meters);
    void setZCameraFocus(double meters);

signals:
    void xRotationChanged(int angle);
    void yRotationChanged(int angle);
    void zRotationChanged(int angle);

    void xCameraPosiChanged(double meters);
    void yCameraPosiChanged(double meters);
    void zCameraPosiChanged(double meters);

    void xCameraFocusChanged(double meters);
    void yCameraFocusChanged(double meters);
    void zCameraFocusChanged(double meters);

    void objectPosiChanged(QVector3D position);
    void objectNumberChanged(int objectNumber);







};

#endif
