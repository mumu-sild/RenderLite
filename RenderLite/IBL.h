#ifndef IBL_H
#define IBL_H

#include "cubemap.h"
#include "texture2d.h"


class IBL
{
public:
    IBL(CubeMap* enviromentCubeMap);

    void getIrrandianceCubeMap(int Size = 32);
    void getPrefilterMap(int Size = 128,unsigned int maxMipLevels = 5,unsigned int sampleCount = 1024);

public:
    CubeMap* envCubeMap;
    CubeMap* irradianceCubeMap;
    CubeMap* prefilterMap;


    //static QOpenGLShaderProgram* irradianceShader;

    static Texture2D* LUT;

};

#endif // IBL_H
