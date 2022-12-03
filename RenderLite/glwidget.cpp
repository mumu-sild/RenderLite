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
    gaussBlur = new GaussianBlur(width(),height());
    ssao = new SSAO(width(),height());

    glClearColor(backgroundDefaultColor.x(),
                 backgroundDefaultColor.y(),
                 backgroundDefaultColor.z(), 1);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_STENCIL_TEST);

    //G-Buffer
    //  位置
    G_Buffer = new QOpenGLFramebufferObject(size(),QOpenGLFramebufferObject::CombinedDepthStencil,GL_TEXTURE_2D,GL_RGBA16F);
    //  法向量
    G_Buffer->addColorAttachment(size(),GL_RGBA);
    //  颜色（HDR）+镜面颜色
    G_Buffer->addColorAttachment(size(),GL_RGBA16F);
    //  高光图（只计算光源物体）
    G_Buffer->addColorAttachment(size(),GL_RGBA16F);
    //  SSAO
    G_Buffer->addColorAttachment(size(),GL_RGBA16F);
    //  viewNormal
    G_Buffer->addColorAttachment(size(),GL_RGB);
    //  设置着色器渲染纹理路径
    G_Buffer->bind();
    GLenum buffers2[] = { GL_COLOR_ATTACHMENT0,
                          GL_COLOR_ATTACHMENT1,
                          GL_COLOR_ATTACHMENT2,
                          GL_COLOR_ATTACHMENT3,
                          GL_COLOR_ATTACHMENT4,
                          GL_COLOR_ATTACHMENT5,};
    core->glDrawBuffers(6, buffers2);
    G_Buffer->release();

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

    LightShader = new QOpenGLShaderProgram();
    LightShader->addShaderFromSourceFile(QOpenGLShader::Vertex,":/G_Buffer.vert");
    LightShader->addShaderFromSourceFile(QOpenGLShader::Fragment,":/G_Buffer.frag");
    LightShader->link();

    PBR_Shader = new QOpenGLShaderProgram();
    PBR_Shader->addShaderFromSourceFile(QOpenGLShader::Vertex,":/PBR_Shader.vert");
    PBR_Shader->addShaderFromSourceFile(QOpenGLShader::Fragment,":/PBR_Shader.frag");
    PBR_Shader->link();

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

//    QVector3D v[3];
//    v[0] = QVector3D(-10,1,-10);
//    v[1] = QVector3D(0,1,10);
//    v[2] = QVector3D(10,1,-10);
//    QVector3D color(0.8,0.8,0.8);
//    Triangle* tri = new Triangle(v,color);
//    scene.Add(tri);
//    scene.shaderPrograms.push_back(shaderSelector.getShader(2));
//    scene.Add(new PointLight(tri->getlightpos(),10));
//    tri->islight = true;


//    rectangle* rec = new rectangle(300,300);
//    rec->model.translate(QVector3D(0,-14,0));
//    scene.Add(rec);
//    scene.Add(shaderSelector.getShader(3));
//    scene.Add(new PointLight(rec->getlightpos(),1));

    Sphere* sphere1 = new Sphere();
    sphere1->model.scale(QVector3D(5,5,5));
    scene.Add(sphere1);
    scene.Add(PBR_Shader);
    scene.Add(new PointLight(sphere1->getlightpos(),10));

    albedoMap = loadtexture("C:/Users/mumu/Desktop/graphics/RenderLite/RenderLite/RenderLite/Picture_source/PBR/rustediron2_basecolor.png");
    normalMap = loadtexture("C:/Users/mumu/Desktop/graphics/RenderLite/RenderLite/RenderLite/Picture_source/PBR/rustediron2_normal.png");
    metallicMap = loadtexture("C:/Users/mumu/Desktop/graphics/RenderLite/RenderLite/RenderLite/Picture_source/PBR/rustediron2_metallic.png");
    roughnessMap = loadtexture("C:/Users/mumu/Desktop/graphics/RenderLite/RenderLite/RenderLite/Picture_source/PBR/rustediron2_roughness.png");

//IBL

    //1. 加载图像  Alexs_Apt_8k.jpg
    QImage data("C:/Users/mumu/Desktop/graphics/RenderLite/outerFile/Alexs_Apartment/Alexs_Apartment/Alexs_Apt_8k.jpg");
    if(data.isNull()){
        qDebug()<<"data is null;";
    }
    else{
        data = data.mirrored();
        equirectangularMap = new QOpenGLTexture(data);
        equirectangularMap->create();
        equirectangularMap->setWrapMode(QOpenGLTexture::DirectionS,QOpenGLTexture::Repeat);
        equirectangularMap->setWrapMode(QOpenGLTexture::DirectionT,QOpenGLTexture::Repeat);
        equirectangularMap->setMinMagFilters(QOpenGLTexture::LinearMipMapLinear,QOpenGLTexture::Linear);
    }

    //          Alexs_Apt_2k.hdr
    stbi_set_flip_vertically_on_load(true);
    int width, height, nrComponents;
    float *data2 = stbi_loadf("C:/Users/mumu/Desktop/graphics/RenderLite/outerFile/Alexs_Apartment/Alexs_Apartment/Alexs_Apt_2k.hdr"\
                            , &width, &height, &nrComponents, 0);
    if (data2)
    {
        glGenTextures(1, &hdrTexture);
        glBindTexture(GL_TEXTURE_2D, hdrTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data2);

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

    unsigned int captureFBO, captureRBO;
    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

    unsigned int envCubemap;
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
    //test
    //glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
//    glViewport(0,0,width()/2,height());
    showPicture(hdrTexture);
//    glViewport(width()/2,0,width(),height());
//    showPicture(equirectangularMap->textureId());



    return;


//  第一次层循环，获取光照信息以及shadow map图
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
        glViewport(0,0,width(),height());
        if(objectNumber&&scene.objects.at(objectNumber-1)->islight){
            showPicture(scene.pointlights.at(objectNumber-1)->depthMapFBO->texture());
        }
        else showPicture(scene.dirlight->depthMapFBO->texture());
        return;
    }

//-----场景渲染----------------------------------------------

//  G-Buffer
    G_Buffer->bind();
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
    }


    //Object Draw
    for(int i=0;i<scene.objects.size();i++){
        glStencilFunc(GL_ALWAYS, i+1, 0xFF);//模板测试始终通过，ref为当前物体编号

        scene.shaderPrograms[i]->bind();
        //model
        scene.shaderPrograms[i]->setUniformValue("model",scene.objects[i]->model.getmodel());
        //color
        if(scene.shaderPrograms[i] == shaderSelector.getShader(shaderTypes::SHADER_COLOR)){//2
            if(scene.objects.at(i)->islight){
                scene.shaderPrograms[i]->setUniformValue("color",scene.pointlights.at(i)->color*scene.pointlights.at(i)->intensity);
            }
            else scene.shaderPrograms[i]->setUniformValue("color",scene.objects.at(i)->color);
        }
        if(scene.shaderPrograms[i] == shaderSelector.getShader(shaderTypes::SHADER_LIGHTCOLOR)){//3
           scene.shaderPrograms[i]->setUniformValue("color",scene.objects[i]->color);
        }
        //PBR
        if(scene.shaderPrograms[i] == PBR_Shader){
            PBR_Shader->setUniformValue("view",maincamera.getViewMetrix());
            PBR_Shader->setUniformValue("projection",projection);

            PBR_Shader->setUniformValue("albedoMap",0);
            glActiveTexture(GL_TEXTURE0);
            albedoMap->bind();

            PBR_Shader->setUniformValue("normalMap",1);
            glActiveTexture(GL_TEXTURE1);
            normalMap->bind();

            PBR_Shader->setUniformValue("metallicMap",2);
            glActiveTexture(GL_TEXTURE2);
            metallicMap->bind();

            PBR_Shader->setUniformValue("roughnessMap",3);
            glActiveTexture(GL_TEXTURE3);
            roughnessMap->bind();

            PBR_Shader->setUniformValue("ao", 1.0f);
            PBR_Shader->setUniformValue("camPos",maincamera.getCameraPos());
            PBR_Shader->setUniformValue("lightPositions[0]",QVector3D(-10,10,10));
            PBR_Shader->setUniformValue("lightColors[0]",QVector3D(1,1,1)*300);
            PBR_Shader->setUniformValue("lightPositions[1]",QVector3D(10,10,10));
            PBR_Shader->setUniformValue("lightColors[1]",QVector3D(1,1,1)*300);
            PBR_Shader->setUniformValue("lightPositions[2]",QVector3D(-10,-10,10));
            PBR_Shader->setUniformValue("lightColors[2]",QVector3D(1,1,1)*300);
            PBR_Shader->setUniformValue("lightPositions[3]",QVector3D(10,-10,10));
            PBR_Shader->setUniformValue("lightColors[3]",QVector3D(1,1,1)*300);
            PBR_Shader->setUniformValue("model", scene.objects[i]->model.getmodel());

//            for (int row = 0; row < 7; ++row) {
//                //PBR_Shader->setUniformValue("metallic", (float)row / (float)7);//每行金属度加大
//                for (int col = 0; col < 7; ++col)
//                {
//                    // 我们钳粗糙度为0.05 - 1.0，因为完美光滑的表面(粗糙度0.0)往往看起来有点偏离直接照明
//                   //PBR_Shader->setUniformValue("roughness", clamp((float)col / (float)7, 0.05f, 1.0f));

//                   QMatrix4x4 PBRTestModel;
//                   PBRTestModel.setToIdentity();
//                   PBRTestModel.translate((col - (7 / 2)) * 2.5,
//                                          (row - (7 / 2)) * 2.5,
//                                          0.0f);
//                   PBR_Shader->setUniformValue("model", PBRTestModel);
//                   scene.objects.at(i)->Draw(*PBR_Shader);
//                }
//            }
        }
        //islight
        scene.shaderPrograms[i]->setUniformValue("islight",scene.objects[i]->islight);
        scene.objects.at(i)->Draw(*scene.shaderPrograms[i]);
    }
    G_Buffer->release();

    //test
//    glBindFramebuffer(GL_FRAMEBUFFER, 0);
//    glViewport(0,0,width(),height());
//    showPicture(G_Buffer->textures().at(2));//color
//    return ;

    //高斯模糊
    QOpenGLFramebufferObject* BluredFBO =  new QOpenGLFramebufferObject(width(),height(),QOpenGLFramebufferObject::NoAttachment,GL_TEXTURE_2D,GL_RGBA16F);
    QOpenGLFramebufferObject::blitFramebuffer(BluredFBO,
                                              gaussBlur->getGaussBlurPhoto(G_Buffer->textures().at(3),15),
                                              GL_COLOR_BUFFER_BIT,GL_LINEAR);
    //unsigned int ID = G_Buffer->textures().at(3);


//-----SSAO-----------------------------------
    //ssao test
    //ssao->generateNoise();
    ssao->SSAOPicture(G_Buffer,projection,maincamera.getViewMetrix());//*maincamera.getViewMetrix()
    //QOpenGLFramebufferObject* SSAOBluredFBO = gaussBlur->getGaussBlurPhoto(ssao->SSAOFBO->texture(),1);
    QOpenGLFramebufferObject* SSAOBluredFBO =  new QOpenGLFramebufferObject(width(),height(),QOpenGLFramebufferObject::NoAttachment,GL_TEXTURE_2D,GL_RGBA16F);
    QOpenGLFramebufferObject::blitFramebuffer(SSAOBluredFBO,
                                              gaussBlur->getGaussBlurPhoto(ssao->SSAOFBO->texture(),1),
                                              GL_COLOR_BUFFER_BIT,GL_LINEAR);
    //ssao Blur 输出
//    glViewport(0,0,width(),height());
//    SSAOBluredFBO->bindDefault();
//    //showPicture(ssao->SSAOFBO->texture());
//    showPicture(SSAOBluredFBO->texture());
//    return ;

    //G-Buffer测试
//    glBindFramebuffer(GL_FRAMEBUFFER, 0);
//    glViewport(0,height()/2,width()/2,height()/2);
//    showPicture(G_Buffer->textures().at(2));//color

//    glViewport(width()/2,height()/2,width()/2,height()/2);//右上
//    showPicture(G_Buffer->textures().at(4));//ViewPosition

//    glViewport(0,0,width()/2,height()/2);
//    showPicture(SSAOBluredFBO->texture());//SSAO
//    //showPicture(G_Buffer->textures().at(2));//color

    /*SSAO输出有时候存在问题*/

//    glViewport(width()/2,0,width()/2,height()/2);
//    //showPicture(G_Buffer->textures().at(5));//ViewNormal
//    showPicture(BluredFBO->texture());//gaussBlur
//    return;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
//-----G_Buffer合成
//    glViewport(0,height()/2,width()/2,height()/2);
    LightShader->bind();
    // G_Buffer输入
    LightShader->setUniformValue("gPosition",0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,G_Buffer->textures().at(0));
    LightShader->setUniformValue("gNormal",1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D,G_Buffer->textures().at(1));
    LightShader->setUniformValue("gAlbedoSpec",2);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D,G_Buffer->textures().at(2));
    LightShader->setUniformValue("gBrightColor",3);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D,BluredFBO->texture());
    LightShader->setUniformValue("SSAO",4);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D,SSAOBluredFBO->texture());
    // 光照信息输入
    scene.dirlight->setShaderPara(LightShader,5);
    int numPointLight = pointLight.size();
    LightShader->setUniformValue("numPointLights",numPointLight);
    for(int i=0;i<pointLight.length();++i){
        pointLight.at(i)->setShaderPara(LightShader,6+i,i);
    }
    // 其他信息
    LightShader->setUniformValue("viewPos",maincamera.getCameraPos());
    LightShader->setUniformValue("shiness",64.0f);
    // 调试参数
    LightShader->setUniformValue("gamma",gamma);
    LightShader->setUniformValue("blinn",blinn);
    LightShader->setUniformValue("toneMapping",toneMapping);
    renderQuad();
    QOpenGLFramebufferObject::blitFramebuffer(nullptr,G_Buffer,GL_STENCIL_BUFFER_BIT);
    QOpenGLFramebufferObject::blitFramebuffer(nullptr,G_Buffer,GL_DEPTH_BUFFER_BIT);

    delete BluredFBO;
    delete SSAOBluredFBO;

    //边框
    if(objectNumber){
        glDisable(GL_CULL_FACE);
        glStencilFunc(GL_NOTEQUAL,objectNumber,0xFF);
        glStencilOp(GL_KEEP,GL_KEEP,GL_REPLACE);

        shaderSelector.getShader(4)->bind();
        m_world = scene.objects[objectNumber-1]->model.getmodel();
        m_world.scale(frameScale);
        shaderSelector.getShader(4)->setUniformValue("model",m_world);
        scene.objects.at(objectNumber-1)->Draw(*shaderSelector.getShader(4));
        glEnable(GL_CULL_FACE);
    }
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

    delete G_Buffer;
    //G-Buffer
    //  位置
    G_Buffer = new QOpenGLFramebufferObject(size(),QOpenGLFramebufferObject::CombinedDepthStencil,GL_TEXTURE_2D,GL_RGBA16F);
    //  法向量
    G_Buffer->addColorAttachment(size(),GL_RGBA);
    //  颜色（HDR）+镜面颜色
    G_Buffer->addColorAttachment(size(),GL_RGBA16F);
    //  高光图（只计算光源物体）
    G_Buffer->addColorAttachment(size(),GL_RGBA16F);
    //  SSAO
    G_Buffer->addColorAttachment(size(),GL_RGBA16F);
    //  viewNormal
    G_Buffer->addColorAttachment(size(),GL_RGB);
    //  设置着色器渲染纹理路径
    G_Buffer->bind();
    GLenum buffers2[] = { GL_COLOR_ATTACHMENT0,
                          GL_COLOR_ATTACHMENT1,
                          GL_COLOR_ATTACHMENT2,
                          GL_COLOR_ATTACHMENT3,
                          GL_COLOR_ATTACHMENT4,
                          GL_COLOR_ATTACHMENT5,};
    core->glDrawBuffers(6, buffers2);
    G_Buffer->release();
    //ssao
    //qDebug()<<"resizeGL";
    gaussBlur->resizeGaussBlurFBO(w,h);
    ssao->resizeSSAO(w,h);
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


