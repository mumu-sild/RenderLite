#ifndef CUBEMAP_H
#define CUBEMAP_H

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions_4_5_Core>
#include <QMatrix4x4>
#include <iostream>
#include <vector>
#include <string>

//类功能:
//  输入:一个等距柱状投影图的路径
//  输出:一个CubeMapTexture的纹理
//
//注意：定义类如果定义到局部空间，则类内的envCubemap只能在局部内使用，类销毁后，envCubemap也会被销毁




class CubeMap : protected QOpenGLFunctions
{
public:
    CubeMap(int CubeSize = 1024);
    CubeMap(std::vector<std::string> faces);
    ~CubeMap();
public:
    int CubeSize;

private:
    GLuint hdrTexture;
    static unsigned int cubeVAO;
    static unsigned int cubeVBO;
    //输出值
    unsigned int envCubemap;

public:
    void getIrrandianceCubMapFromCube(unsigned int SourceMap);
    void RenderToCube(const char* path);
    unsigned int getCubeMapID();

public:
    static void renderCube();


};

#endif // CUBEMAP_H
