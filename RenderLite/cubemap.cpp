#include "camera.h"
#include "cubemap.h"
#include "stb_image.h"
#include <iostream>
#include <math.h>

using namespace std;

bool CubeMap::isinit = false;
unsigned int CubeMap::cubeVAO = 0;
unsigned int CubeMap::cubeVBO = 0;
QMatrix4x4 CubeMap::captureProjection;
QMatrix4x4 CubeMap::lookatMatrix[6];
QOpenGLShaderProgram* CubeMap::skyboxShader;

QOpenGLFunctions_4_5_Core * core;

/* @param
 *     CubeSize:��պеĳ���
 *     mipmap:�Ƿ�����mipmap
 */
CubeMap::CubeMap(int CubeSize, bool mipmap) : CubeSize(CubeSize)
{

    initializeOpenGLFunctions();
    if(!CubeMap::isinit) CubeMap::init();


    //CubeMap �����ռ�ͼ
    glGenTextures(1, &envCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        // note that we store each face with 16 bit floating point values
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F,
                     CubeSize, CubeSize, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    if(!mipmap)
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);//GL_NEAREST
    else
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER,GL_LINEAR);

    if(mipmap) {
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

        this->mipmap = -1;
        while(CubeSize > 0){
            CubeSize >>= 1;
            ++this->mipmap;
        }
    }
}

CubeMap::CubeMap(std::vector<std::string> faces, bool mipmap)
{
    initializeOpenGLFunctions();
    if(!CubeMap::isinit) CubeMap::init();

    glGenTextures(1, &envCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

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
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    if(!mipmap) glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    else glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    if(mipmap){
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

        this->mipmap = -1;
        while(CubeSize > 0){
            CubeSize >>= 1;
            ++this->mipmap;
        }
    }

    if(width == height)CubeSize = width;
    else {
        CubeSize = width;
        std::cout << "Cubemap texture is not a ������: "  << std::endl;
    }

}



void CubeMap::getIrrandianceCubMapFromCube(unsigned int SourceMap)
{
    glDisable(GL_CULL_FACE);
    //Buffer
    unsigned int captureFBO, captureRBO;
    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, CubeSize, CubeSize);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);
    //��Ⱦ��envCubemap��ʹ��irradianceShader

    //Shader
    QOpenGLShaderProgram irradianceShader;
    irradianceShader.addShaderFromSourceFile(QOpenGLShader::Vertex,":/cubemap.vert");
    irradianceShader.addShaderFromSourceFile(QOpenGLShader::Fragment,":/irradiance_convolution.frag");
    irradianceShader.link();

    irradianceShader.bind();
    irradianceShader.setUniformValue("projection",captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, SourceMap);

    glViewport(0,0,CubeSize,CubeSize);

    glBindFramebuffer(GL_FRAMEBUFFER,captureFBO);
    for(unsigned int i = 0; i < 6; ++i){
        irradianceShader.setUniformValue("view",lookatMatrix[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,\
                               GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, \
                               envCubemap,0);
        glClear(GL_COLOR_BUFFER_BIT |  GL_DEPTH_BUFFER_BIT);
        renderCube();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glDeleteFramebuffers(1,&captureFBO);
    glDeleteRenderbuffers(1,&captureRBO);
}

void CubeMap::renderHDRTextureToCube(const char* path)
{
    glDisable(GL_CULL_FACE);
//����ͼƬ
    GLuint hdrTexture;
    stbi_set_flip_vertically_on_load(true);
    int Texwidth, Texheight, TexnrComponents;
    float *data = stbi_loadf(path, &Texwidth, &Texheight, &TexnrComponents, 0);
    if (data)
    {
        glGenTextures(1, &hdrTexture);
        glBindTexture(GL_TEXTURE_2D, hdrTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, Texwidth, Texheight, 0, GL_RGB, GL_FLOAT, data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Failed to load <HDR> image." << std::endl;
        return;
    }

    stbi_set_flip_vertically_on_load(false);

//ת��ΪCubeMap
    unsigned int captureFBO, captureRBO;

    //FBO ����֡���桢����Ȼ����ģ�建��
    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, CubeSize, CubeSize);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

     //Shader
     QOpenGLShaderProgram equirectangularToCubemapShader;
     equirectangularToCubemapShader.addShaderFromSourceFile(QOpenGLShader::Vertex,":/cubemap.vert");
     equirectangularToCubemapShader.addShaderFromSourceFile(QOpenGLShader::Fragment,":/rectTocube.frag");
     equirectangularToCubemapShader.link();
     equirectangularToCubemapShader.bind();
     equirectangularToCubemapShader.setUniformValue("projection", captureProjection);
     glActiveTexture(GL_TEXTURE0);
     glBindTexture(GL_TEXTURE_2D, hdrTexture);

     // pbr: convert HDR equirectangular environment map to cubemap equivalent
     // ��Ⱦ
     glViewport(0,0,CubeSize,CubeSize);
     glBindFramebuffer(GL_FRAMEBUFFER,captureFBO);
     for (unsigned int i = 0; i < 6; ++i)
     {
         equirectangularToCubemapShader.setUniformValue("view", lookatMatrix[i]);
         glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
         glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
         renderCube();
     }
     glBindFramebuffer(GL_FRAMEBUFFER, 0);

     glDeleteFramebuffers(1,&captureFBO);
     glDeleteRenderbuffers(1,&captureRBO);
}

/*
 *  ���ܣ�ͨ���������պУ��������ɸ���պе�Ԥ�˲�HDR������ͼ
 *
 *  ���룺��ҪԤ�˲������ͼ�ı�ţ�Ԥ������mipmap����,�����ز�����
 *  ����������envCubeMap�����˼���õ���Ԥ�˲�HDR������ͼ�ı��
 */
void CubeMap::getIBLprefilterMapFromEnvCubeMap(unsigned int CubeMap,unsigned int maxMipLevels,unsigned int sampleCount)
{
     mipmap = (int)maxMipLevels;

     glDisable(GL_CULL_FACE);
    //��һ������������Ԥ�˲�Shader
    QOpenGLShaderProgram prefilterShader;
    prefilterShader.addShaderFromSourceFile(QOpenGLShader::Vertex,":/cubemap.vert");
    prefilterShader.addShaderFromSourceFile(QOpenGLShader::Fragment,":/prefilterMap.frag");
    prefilterShader.link();

    //�ڶ�����FBO ����֡���桢����Ȼ����ģ�建��
    unsigned int captureFBO, captureRBO;
    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, CubeSize, CubeSize);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

    //������������Uniform����������Ⱦ����ǰCubemap
    prefilterShader.bind();
    prefilterShader.setUniformValue("projection", captureProjection);   //vert
    prefilterShader.setUniformValue("environmentMap", 0);               //frag
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, CubeMap);//�˴�����CubeMap
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

    for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
    {
        // 1.�����i���mipmap��С
        unsigned int mipi  = CubeSize >> mip;
            //mipmap��i��ĳ���Ϊ ��0���С * 0.5^mip
            // == CubeSize * pow(0.5, mip)
            // == CubeSize >> mip

        // 2.���ø�mip�����Ⱦ�����С
        glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipi, mipi);
        glViewport(0, 0, mipi, mipi);

        // 3.����mipmap�Ĳ㼶ѡ��Ԥ�˲����˲�ģ����
        float roughness = (float)mip / (float)(maxMipLevels - 1);
        prefilterShader.setUniformValue("roughness", roughness);        //frag
        prefilterShader.setUniformValue("sample_count", sampleCount);   //frag


        // 4.��Ⱦ�õ��ò㼶��Ԥ�˲���������ͼ
        for (unsigned int i = 0; i < 6; ++i)
        {
            prefilterShader.setUniformValue("view", lookatMatrix[i]);   //vert
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                   GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, mip);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            renderCube();
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glDeleteFramebuffers(1,&captureFBO);
    glDeleteRenderbuffers(1,&captureRBO);

}

unsigned int CubeMap::getCubeMapID()
{
    return envCubemap;
}

void CubeMap::setToLINEAR()
{
    if(!mipmap) glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    else glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
}

void CubeMap::renderToSkyBox(const camera &maincamera)
{
    glDepthFunc(GL_LEQUAL);
    //glDisable(GL_CULL_FACE);

    skyboxShader->bind();
    skyboxShader->setUniformValue("projection", maincamera.projection);
    skyboxShader->setUniformValue("view",  maincamera.getViewMetrix());
    skyboxShader->setUniformValue("skybox", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP,envCubemap);
    //envCubemap cubemapTexture irradianceMap prefilterMap->getCubeMapID()

    //prefilterMap->setToNEAREST();

    //�˴���Ҫ�޸�
    skyboxShader->setUniformValue("mipmapLevel", 0.0f);
    renderCube();

    glDepthFunc(GL_LESS);
}

void CubeMap::setToNEAREST()
{
    if(!mipmap) glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    else glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);

}


void CubeMap::renderCube()
{

    core = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_5_Core>();
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
        core->glGenBuffers(1, &cubeVBO);
        // fill buffer
        core->glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        core->glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // link vertex attributes
        core->glBindVertexArray(cubeVAO);
        core->glEnableVertexAttribArray(0);
        core->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        core->glEnableVertexAttribArray(1);
        core->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        core->glEnableVertexAttribArray(2);
        core->glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        core->glBindBuffer(GL_ARRAY_BUFFER, 0);
        core->glBindVertexArray(0);
    }
    // render Cube
    core->glBindVertexArray(cubeVAO);
    core->glDrawArrays(GL_TRIANGLES, 0, 36);
    core->glBindVertexArray(0);

}

void CubeMap::init()
{
    cout<<"CubeMap::isinit"<<endl;
    captureProjection.perspective(90.0f, 1.0f, 0.1f, 10.0f);
    lookatMatrix[0].lookAt(QVector3D(0.0f, 0.0f, 0.0f), QVector3D( 1.0f,  0.0f,  0.0f), QVector3D(0.0f, -1.0f,  0.0f));
    lookatMatrix[1].lookAt(QVector3D(0.0f, 0.0f, 0.0f), QVector3D(-1.0f,  0.0f,  0.0f), QVector3D(0.0f, -1.0f,  0.0f));
    lookatMatrix[2].lookAt(QVector3D(0.0f, 0.0f, 0.0f), QVector3D( 0.0f,  1.0f,  0.0f), QVector3D(0.0f,  0.0f,  1.0f));
    lookatMatrix[3].lookAt(QVector3D(0.0f, 0.0f, 0.0f), QVector3D( 0.0f, -1.0f,  0.0f), QVector3D(0.0f,  0.0f, -1.0f));
    lookatMatrix[4].lookAt(QVector3D(0.0f, 0.0f, 0.0f), QVector3D( 0.0f,  0.0f,  1.0f), QVector3D(0.0f, -1.0f,  0.0f));
    lookatMatrix[5].lookAt(QVector3D(0.0f, 0.0f, 0.0f), QVector3D( 0.0f,  0.0f, -1.0f), QVector3D(0.0f, -1.0f,  0.0f));
    isinit = true;

    //��Ⱦ��պе�shader
    skyboxShader = new QOpenGLShaderProgram();
    skyboxShader->addShaderFromSourceFile(QOpenGLShader::Vertex,":/skybox.vert");
    skyboxShader->addShaderFromSourceFile(QOpenGLShader::Fragment,":/skybox.frag");
    skyboxShader->link();
}

CubeMap::~CubeMap()
{
    glDeleteTextures(1,&envCubemap);
}
