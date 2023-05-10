#ifndef TEXTURE2D_H
#define TEXTURE2D_H

#include <QOpenGLFunctions>
#include <QOpenGLFunctions_4_5_Core>

class Texture2D : public QOpenGLFunctions_4_5_Core
{
public:
    Texture2D(int width,int height, GLint type, bool mipmap = false);
private:
    int width;
    int height;
public:
    unsigned int Texture2D_ID;

public:
    void generateTextureLUT();
    void renderQuad();
};

#endif // TEXTURE2D_H
