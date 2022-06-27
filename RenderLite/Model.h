#ifndef MODEL_H
#define MODEL_H

#include <QVector>
#include <QDir>
#include <QOpenGLTexture>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"

using namespace std;

//struct Material{
//    unsigned int id;
//    QVector3D diffuseColor;
//    QVector3D ambientColor;
//    QVector3D specularColor;

//    //��ͼ������-1��ʾû�д���ͼ
//    int diffuseTexture;
//    int specularTexture;
//    int normalTexture;
//    int heightTexture;
//    float shininess;
//    //������͸��

//};

class Model
{
public:
    QVector<Texture*> textures_loaded;	//�洢��ĿǰΪֹ���ص����������Ż���ȷ����������س���һ�Ρ�
    QVector<Mesh*>    meshes;           //�洢����
    QDir directory;                     //ģ������·��Ŀ¼

    Model(QString const& path);
    void Draw(QOpenGLShaderProgram& shader);
    ~Model();
private:
    int meshCount=0;
    int textureCount=0;
    int materialCount=0;


private:
    // ����ģ��
    void loadModel(const QString& path);

    // �ݹ�������
    void processNode(aiNode* node, const aiScene* scene);
    //��������
    Mesh *processMesh(aiMesh* mesh, const aiScene* scene);
    //�����������
    vector<Texture*> loadMaterialTextures(aiMaterial* mat, aiTextureType type, QString typeName);

};



#endif // MODEL_H
