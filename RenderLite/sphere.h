#ifndef SPHERE_H
#define SPHERE_H

#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>

#include "Object.h"
#include "Texture.h"
#include "Vertex.h"

class Sphere : public Object , public QOpenGLFunctions
{
private:
    static bool isinitial;
    static QVector<Vertex> v;
    static QVector<unsigned int> indices;
    static unsigned int indexCount;

    QOpenGLVertexArrayObject VAO;
    QOpenGLBuffer VBO;
    QOpenGLBuffer EBO;
    QVector<Texture*> textures;
public:
    Sphere();

    void Draw(QOpenGLShaderProgram& shader)override;
    QVector3D getlightpos()override;
    QVector3D getlightNormal()override;
    float getArea()override;

    ~Sphere();

private:
    void setupVAO();
};

#endif // SPHERE_H
