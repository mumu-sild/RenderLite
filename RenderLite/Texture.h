#ifndef TEXTURE_H
#define TEXTURE_H
#include <QOpenGLTexture>
#include <QString>

struct Texture {
    QOpenGLTexture texture;
    QString path;
    QString type;
    Texture():texture(QOpenGLTexture::Target2D){
            texture.create();
            texture.setWrapMode(QOpenGLTexture::DirectionS,QOpenGLTexture::Repeat);
            texture.setWrapMode(QOpenGLTexture::DirectionT,QOpenGLTexture::Repeat);
            texture.setMinMagFilters(QOpenGLTexture::LinearMipMapLinear,QOpenGLTexture::Linear);
    }
};

#endif // TEXTURE_H
