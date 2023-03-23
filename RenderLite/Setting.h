#ifndef SETTING_H
#define SETTING_H

#include <QVector3D>

const int windowsMinimumSize = 50;
const int windowsNormalSize = 400;
const QVector3D backgroundDefaultColor(0.0, 0.0, 0.0);
const QVector3D ObjectDefaultColor(0.8,0.8,0.8);
const QVector3D camereInitialPosition(0.0f,0.0f,0.0f);
const QVector3D cameraInitialFocus(0.0f,0.0f,-1.0f);
const QVector3D cameraInitUp(0.0,1.0,0.0);
const int defaultshader = 2;
const int mouseClickOffset = 3;

const float pointLightDefaultAmbient = 0.05;
const float PointLightDefaultDiffuse = 0.08f;
const float PointLightDefaultSpecular = 0.3f;
const float PointLightDefaultConstant = 0.001f;
const float PointLightDefaultLinear = 0.009f;
const float PointLightDefaultQuadratic=0.0001;
const float DirLightDefaultAmbient = 0.4f;
const float DirLightDefaultDiffuse = 0.5f;
const float DirLightDefaultSpecular = 0.5f;

const QVector3D frameScale(1.01,1.01,1.01);

const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;


#endif // SETTING_H
