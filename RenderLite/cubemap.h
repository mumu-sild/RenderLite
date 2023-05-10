#ifndef CUBEMAP_H
#define CUBEMAP_H

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions_4_5_Core>
#include <QMatrix4x4>
#include <iostream>
#include <vector>
#include <string>


#include "camera.h"
//类功能:
//  输入:一个等距柱状投影图的路径
//  输出:一个CubeMapTexture的纹理
//
//注意：定义类如果定义到局部空间，则类内的envCubemap只能在局部内使用，类销毁后，envCubemap也会被销毁

class CubeMap : protected QOpenGLFunctions
{
public:
    CubeMap(int CubeSize = 1024 , bool mipmap = false);
    CubeMap(std::vector<std::string> faces, bool mipmap = false);
    ~CubeMap();
public:
    int CubeSize;
    int mipmap = 0;

private:
    static bool isinit;
    static unsigned int cubeVAO;
    static unsigned int cubeVBO;
    static QMatrix4x4 captureProjection;
    static QMatrix4x4 lookatMatrix[6];

    static QOpenGLShaderProgram* skyboxShader;

private:
    //输出值
    unsigned int envCubemap;

public:
    void getIrrandianceCubMapFromCube(unsigned int SourceMap);
    void renderHDRTextureToCube(const char* path);
    void getIBLprefilterMapFromEnvCubeMap(unsigned int CubeMap,unsigned int maxMipLevels = 5,unsigned int sampleCount = 1024);
    unsigned int getCubeMapID();
    void setToNEAREST();
    void setToLINEAR();
    void renderToSkyBox(const camera &maincamera);

private:
    static void renderCube();
    static void init();


};

#endif // CUBEMAP_H
