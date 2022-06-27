#ifndef MESH_H
#define MESH_H

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QVector>
#include <QOpenGLTexture>

//#include "vec.h"
//#include "QShader.h"
using namespace std;

#define MAX_BONE_INFLUENCE 4

struct Vertex {
    // position
    QVector3D Position;
    // normal
    QVector3D Normal;
    // texCoords
    QVector2D TexCoords;
    // tangent正切
    QVector3D Tangent;
    // bitangent双切线
    QVector3D Bitangent;
    //bone indexes which will influence this vertex
    int m_BoneIDs[MAX_BONE_INFLUENCE];
    //weights from each bone
    float m_Weights[MAX_BONE_INFLUENCE];
};

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
