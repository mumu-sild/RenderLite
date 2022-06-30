#ifndef MODEL_H
#define MODEL_H

#include <QVector>
#include <QDir>
#include <QOpenGLTexture>
#include <QMatrix4x4>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


#include "Object.h"
#include "Mesh.h"
#include "triangle.h"

using namespace std;

//struct Material{
//    unsigned int id;
//    QVector3D diffuseColor;
//    QVector3D ambientColor;
//    QVector3D specularColor;

//    //贴图索引，-1表示没有此贴图
//    int diffuseTexture;
//    int specularTexture;
//    int normalTexture;
//    int heightTexture;
//    float shininess;
//    //不考虑透明

//};

class Model:public Object
{
public:

    QVector<Texture*> textures_loaded;	//存储到目前为止加载的所有纹理，优化以确保纹理不会加载超过一次。
    QVector<Mesh*>    meshes;           //存储网格
    QDir directory;                     //模型所在路径目录
    QVector<Triangle*> triangles;

    Model(QString const& path);
    void Draw(QOpenGLShaderProgram& shader);

    ~Model();
private:
    int meshCount=0;
    int textureCount=0;
    int materialCount=0;


private:
    // 加载模型
    void loadModel(const QString& path);

    // 递归遍历结点
    void processNode(aiNode* node, const aiScene* scene);
    //加载网络
    Mesh *processMesh(aiMesh* mesh, const aiScene* scene);
    //加载纹理材质
    vector<Texture*> loadMaterialTextures(aiMaterial* mat, aiTextureType type, QString typeName);

    //加载三角形数据
    void loadTriangle();
};



#endif // MODEL_H
