#include "cubemap.h"
#include "stb_image.h"
#include <iostream>

using namespace std;

unsigned int CubeMap::cubeVAO = 0;
unsigned int CubeMap::cubeVBO = 0;
QOpenGLFunctions_4_5_Core * core;

CubeMap::CubeMap(int CubeSize) : CubeSize(CubeSize)
{

    initializeOpenGLFunctions();

    //CubeMap 创建空间图
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
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

CubeMap::CubeMap(std::vector<std::string> faces)
{
    initializeOpenGLFunctions();

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
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    if(width == height)CubeSize = width;
    else {
        CubeSize = width;
        std::cout << "Cubemap texture is not a 正方形: "  << std::endl;
    }

}



void CubeMap::getIrrandianceCubMapFromCube(unsigned int SourceMap)
{
        //Matrix
        QMatrix4x4 captureProjection;
        QMatrix4x4 lookatMatrix[6];
        captureProjection.perspective(90.0f, 1.0f, 0.1f, 10.0f);
        lookatMatrix[0].lookAt(QVector3D(0.0f, 0.0f, 0.0f), QVector3D( 1.0f,  0.0f,  0.0f), QVector3D(0.0f, -1.0f,  0.0f));
        lookatMatrix[1].lookAt(QVector3D(0.0f, 0.0f, 0.0f), QVector3D(-1.0f,  0.0f,  0.0f), QVector3D(0.0f, -1.0f,  0.0f));
        lookatMatrix[2].lookAt(QVector3D(0.0f, 0.0f, 0.0f), QVector3D( 0.0f,  1.0f,  0.0f), QVector3D(0.0f,  0.0f,  1.0f));
        lookatMatrix[3].lookAt(QVector3D(0.0f, 0.0f, 0.0f), QVector3D( 0.0f, -1.0f,  0.0f), QVector3D(0.0f,  0.0f, -1.0f));
        lookatMatrix[4].lookAt(QVector3D(0.0f, 0.0f, 0.0f), QVector3D( 0.0f,  0.0f,  1.0f), QVector3D(0.0f, -1.0f,  0.0f));
        lookatMatrix[5].lookAt(QVector3D(0.0f, 0.0f, 0.0f), QVector3D( 0.0f,  0.0f, -1.0f), QVector3D(0.0f, -1.0f,  0.0f));
        //Buffer
        unsigned int captureFBO, captureRBO;
        glGenFramebuffers(1, &captureFBO);
        glGenRenderbuffers(1, &captureRBO);
        glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
        glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, CubeSize, CubeSize);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);
        //渲染到envCubemap。使用irradianceShader

        QOpenGLShaderProgram irradianceShader;
        irradianceShader.addShaderFromSourceFile(QOpenGLShader::Vertex,":/rectTocube.vert");
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
}



void CubeMap::RenderToCube(const char* path)
{
//载入图片
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
    }

    stbi_set_flip_vertically_on_load(false);

//转化为CubeMap
    unsigned int captureFBO, captureRBO;
    QMatrix4x4 captureProjection;
    QMatrix4x4 lookatMatrix[6];
    //FBO 创建帧缓存、绑定深度缓存和模板缓存
    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, CubeSize, CubeSize);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

    captureProjection.perspective(90.0f, 1.0f, 0.1f, 10.0f);

    lookatMatrix[0].lookAt(QVector3D(0.0f, 0.0f, 0.0f), QVector3D( 1.0f,  0.0f,  0.0f), QVector3D(0.0f, -1.0f,  0.0f));
    lookatMatrix[1].lookAt(QVector3D(0.0f, 0.0f, 0.0f), QVector3D(-1.0f,  0.0f,  0.0f), QVector3D(0.0f, -1.0f,  0.0f));
    lookatMatrix[2].lookAt(QVector3D(0.0f, 0.0f, 0.0f), QVector3D( 0.0f,  1.0f,  0.0f), QVector3D(0.0f,  0.0f,  1.0f));
    lookatMatrix[3].lookAt(QVector3D(0.0f, 0.0f, 0.0f), QVector3D( 0.0f, -1.0f,  0.0f), QVector3D(0.0f,  0.0f, -1.0f));
    lookatMatrix[4].lookAt(QVector3D(0.0f, 0.0f, 0.0f), QVector3D( 0.0f,  0.0f,  1.0f), QVector3D(0.0f, -1.0f,  0.0f));
    lookatMatrix[5].lookAt(QVector3D(0.0f, 0.0f, 0.0f), QVector3D( 0.0f,  0.0f, -1.0f), QVector3D(0.0f, -1.0f,  0.0f));

     //Shader
     QOpenGLShaderProgram equirectangularToCubemapShader;
     equirectangularToCubemapShader.addShaderFromSourceFile(QOpenGLShader::Vertex,":/rectTocube.vert");
     equirectangularToCubemapShader.addShaderFromSourceFile(QOpenGLShader::Fragment,":/rectTocube.frag");
     equirectangularToCubemapShader.link();
     equirectangularToCubemapShader.bind();
     equirectangularToCubemapShader.setUniformValue("projection", captureProjection);
     glActiveTexture(GL_TEXTURE0);
     glBindTexture(GL_TEXTURE_2D, hdrTexture);

     // pbr: convert HDR equirectangular environment map to cubemap equivalent
     // 渲染
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
}

unsigned int CubeMap::getCubeMapID()
{
    return envCubemap;
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

CubeMap::~CubeMap()
{

}
