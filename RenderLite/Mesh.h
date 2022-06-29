#ifndef MESH_H
#define MESH_H

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QVector>
#include <QOpenGLTexture>

#include "Texture.h"
#include "Vertex.h"

using namespace std;

#define MAX_BONE_INFLUENCE 4



class Mesh: public QOpenGLFunctions {
public:
    // mesh Data
    QVector<Vertex>       vertices; //顶点
    QVector<unsigned int> indices;  //索引
    QVector<Texture*>      textures;//纹理
    //QMatrix4x4 model;               //模型矩阵

private:
    QOpenGLVertexArrayObject VAO;
    QOpenGLBuffer VEO;
    QOpenGLBuffer VBO;

public:
    // constructor
    Mesh(QVector<Vertex> vertices, QVector<unsigned int> indices, QVector<Texture*> textures);
    void Draw(QOpenGLShaderProgram &shader);
    ~Mesh();

private:
    // initializes all the buffer objects/arrays
    void setupMesh();
};



#endif // MESH_H
