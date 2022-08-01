#ifndef RECTANGLE_H
#define RECTANGLE_H

#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include "Object.h"
#include "Vertex.h"
#include "Texture.h"

class rectangle : public Object,QOpenGLFunctions
{
private:
    QVector<Vertex> v;
    QVector<unsigned int> indices;  //Ë÷Òý
    QVector<Texture*> textures;
    QVector3D color;
    float width;
    float height;

private:
    QOpenGLVertexArrayObject VAO;
    QOpenGLBuffer VBO;
    QOpenGLBuffer EBO;
public:
    rectangle(float width = 1,float height = 1,QVector3D color = QVector3D(0.1,0.1,0.1));
    void Draw(QOpenGLShaderProgram& shader)override;
    QVector3D getlightpos()override;
    QVector3D getlightNormal()override;
    float getArea()override;
    ~rectangle();
private:
    void setupVAO();
};

#endif // RECTANGLE_H
