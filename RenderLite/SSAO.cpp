#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>

#include "SSAO.h"
#include "Global.h"

SSAO::SSAO(int w, int h):texture(QOpenGLTexture::Target2D)
{
    //qDebug()<<"initialSSAO";
    SSAOFBO = new QOpenGLFramebufferObject(w,h,QOpenGLFramebufferObject::QOpenGLFramebufferObject::NoAttachment,GL_TEXTURE_2D,GL_RGBA);
    SSAOShader = new QOpenGLShaderProgram();
    SSAOShader->addShaderFromSourceFile(QOpenGLShader::Vertex,":/SSAOShader.vert");
    SSAOShader->addShaderFromSourceFile(QOpenGLShader::Fragment,":/SSAOShader.frag");
    SSAOShader->link();
    initializeOpenGLFunctions();
    generateKernel();
    generateNoise();
}

void SSAO::generateKernel()
{
    for(int i=0;i<64;++i){
        QVector3D sample(random_double(-1,1),random_double(-1,1),random_double(0,1));
        sample.normalize();//生成一个半球面的随机向量
        sample *= random_double(0,1);//将随机向量均匀分布在半球体内
        double scale = double(i)/64.0;
        //scale = 0.1 + scale*scale*(0.9);//将随机向量再次缩放，使绝大多数采样点接近原点
        sample *= scale;
        //qDebug()<<"Kernel"<<i<<"="<<sample;
        SSAOKernel.push_back(sample);
    }
}

void SSAO::generateNoise()
{
    QVector<QVector3D> SSAONoise;
    for(int i=0;i<16;++i){
        QVector3D noise(random_double(-1,1),random_double(-1,1),0.0);
        SSAONoise.push_back(noise);
        //qDebug()<<i<<"Noise="<<noise;
    }
//    {
//        GLuint noiseTexture;
//        glGenTextures(1, &noiseTexture);
//        glBindTexture(GL_TEXTURE_2D, noiseTexture);
//        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 4, 4, 0, GL_RGB, GL_FLOAT, &SSAONoise[0]);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//        glActiveTexture(GL_TEXTURE0);
//        glBindTexture(GL_TEXTURE_2D,noiseTexture);
//    }

    texture.create();
    if(!texture.isStorageAllocated()){
        texture.setSize(4,4,1);
        texture.setFormat(QOpenGLTexture::RGBFormat);
        texture.allocateStorage(QOpenGLTexture::RGB,QOpenGLTexture::Float32);
        texture.setData(0,QOpenGLTexture::RGB,QOpenGLTexture::Float32,(void*)SSAONoise.data());
        texture.setMinMagFilters(QOpenGLTexture::Nearest,QOpenGLTexture::Nearest);
        texture.setWrapMode(QOpenGLTexture::Repeat);
    }
//    glActiveTexture(GL_TEXTURE0);
//    texture.bind();

//    //test
//    debugShader = new QOpenGLShaderProgram();
//    debugShader->addShaderFromSourceFile(QOpenGLShader::Vertex,":/map_depth.vert");
//    debugShader->addShaderFromSourceFile(QOpenGLShader::Fragment,":/map_depth.frag");
//    debugShader->link();

//    debugShader->bind();//shader
//    debugShader->setUniformValue("depthMap",0);
//    renderQuad();
}

void SSAO::setUniform(QOpenGLFramebufferObject* GBuffer,QMatrix4x4 projection,QMatrix4x4 view)
{
    SSAOShader->bind();

    SSAOShader->setUniformValue("gPositionDepth",0);//视口坐标
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,GBuffer->textures().at(4));

    SSAOShader->setUniformValue("gNormal",1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D,GBuffer->textures().at(5));

    SSAOShader->setUniformValue("texNoise",2);
    glActiveTexture(GL_TEXTURE2);
    texture.bind();

    for(int i=0;i<64;++i){
        SSAOShader->setUniformValue(QString("samples[%1]").arg(i).toStdString().c_str(),SSAOKernel.at(i));
    }

    //SSAOShader->setUniformValueArray("samples",SSAOKernel.data(),64);
    SSAOShader->setUniformValue("projection",projection);
    SSAOShader->setUniformValue("view",view);
    SSAOShader->setUniformValue("whichDepth",debug);
    //qDebug()<<"debug"<<debug;
    // qDebug()<<"projection:\n"<<projection;
    SSAOShader->setUniformValue("width",SSAOFBO->width());
    SSAOShader->setUniformValue("height",SSAOFBO->height());
}

QOpenGLFramebufferObject *SSAO::SSAOPicture(QOpenGLFramebufferObject* GBuffer,QMatrix4x4 projection,QMatrix4x4 view)
{
    SSAOFBO->bind();
    //SSAOFBO->bindDefault();
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT);
    setUniform(GBuffer,projection,view);
    renderQuad();
    SSAOShader->release();
    SSAOFBO->release();
    return SSAOFBO;
}

void SSAO::renderQuad()
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

void SSAO::resizeSSAO(int w, int h)
{
    delete SSAOFBO;
    SSAOFBO = new QOpenGLFramebufferObject(w,h,QOpenGLFramebufferObject::QOpenGLFramebufferObject::NoAttachment,GL_TEXTURE_2D,GL_RGBA);
}

SSAO::~SSAO()
{
    delete SSAOFBO;
    delete SSAOShader;
    texture.destroy();
    //delete debugShader;
}
