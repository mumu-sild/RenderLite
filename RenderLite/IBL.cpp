#include "IBL.h"

bool isinitLUT = false;
Texture2D* IBL::LUT;

IBL::IBL(CubeMap *enviromentCubeMap):envCubeMap(enviromentCubeMap)
{
    if(!isinitLUT){
        LUT = new Texture2D(512,512,GL_RG16F,false);
        LUT->generateTextureLUT();
        isinitLUT = true;
    }
}

void IBL::getIrrandianceCubeMap(int Size)
{
    //辐照度立方体贴图
    irradianceCubeMap = new CubeMap(Size);
    irradianceCubeMap->getIrrandianceCubMapFromCube(envCubeMap->getCubeMapID());//envCubemap cubemapTexture
}

void IBL::getPrefilterMap(int Size ,unsigned int maxMipLevels, unsigned int sampleCount)
{
    prefilterMap = new CubeMap(Size,true);
    prefilterMap->getIBLprefilterMapFromEnvCubeMap(envCubeMap->getCubeMapID(),maxMipLevels,sampleCount);
}
