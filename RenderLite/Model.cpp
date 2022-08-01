#include "Model.h"
#include <QFile>
#include <QDir>
#include <QDialog>

/*构建path路径下的模型*/
Model::Model(const QString &path):directory(path)
{

    loadModel(path);
    //loadTriangle();
}

Model::~Model()
{
    for(int i=0;i<meshes.size();i++){
        delete meshes[i];
    }
    for(int i = 0; i< textures_loaded.size();i++){
        delete textures_loaded[i];
    }
    for(int i = 0; i<triangles.size();i++){
        delete triangles[i];
    }
}

/*从Importer中读取数据，并构建结点*/
void Model::loadModel(const QString &path)
{
    //从Assimp类中读取数据
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path.toLatin1(),
                                aiProcess_Triangulate |
                                aiProcess_GenSmoothNormals |
                                aiProcess_FlipUVs |
                                aiProcess_CalcTangentSpace);
    // scene场景包含导入的模型/场景中的所有数据。
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
    {
        qDebug() << "ERROR::ASSIMP:: " << importer.GetErrorString();
        return;
    }
    qDebug()<<directory.absolutePath()<<"加载成功";
    qDebug() <<"网格："<<scene->mNumMeshes;
    qDebug() <<"材质："<<scene->mNumMaterials;
    qDebug() <<"纹理："<<scene->mNumTextures;
    qDebug() <<"动画："<<scene->mNumAnimations;
    directory.cdUp();
    qDebug() <<"directory:"<<directory.absolutePath();

    processNode(scene->mRootNode, scene);
    //processMaterials(scene);
}

/*遍历该模型的所有结点*/
void Model::processNode(aiNode *node, const aiScene *scene)
{
    //处理位于当前节点的每个网络
    for (unsigned int i = 0; i < node->mNumMeshes; i++)//node->nNumMeshed中存储了Mesh数量
    {
        // 节点对象只包含索引来索引场景中的实际对象.
        // 场景包含了所有的数据，节点只是为了保持内容的组织(如节点之间的关系)。
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }
    //在我们处理完所有的网格(如果有的话)之后，我们再递归地处理每个子节点
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }

}

/*
 * 将每个结点的信息（顶点信息，纹理信息）导出，存储在meshes中，
 * 再将所有纹理信息存储在texture_loaded中避免重复
*/
Mesh *Model::processMesh(aiMesh *mesh, const aiScene *scene)
{
    // data to fill
    QVector<Vertex> vertices;//顶点，法向量，纹理坐标
    QVector<unsigned int> indices;

    // walk through each of the mesh's vertices
    //载入  vector<Vertex> vertices  所有点集（点，法线，纹理坐标等）
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        QVector3D vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
        // positions
        vector[0] = mesh->mVertices[i].x;
        vector[1] = mesh->mVertices[i].y;
        vector[2] = mesh->mVertices[i].z;
        vertex.Position = vector;
        // normals
        if (mesh->HasNormals())
        {
            vector[0] = mesh->mNormals[i].x;
            vector[1] = mesh->mNormals[i].y;
            vector[2] = mesh->mNormals[i].z;
            vertex.Normal = vector;
        }
        // texture coordinates
        QVector2D vec;
        if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
        {
            // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
            // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
            vec[0] = mesh->mTextureCoords[0][i].x;
            vec[1] = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;

            // tangent
            vector[0] = mesh->mTangents[i].x;
            vector[1] = mesh->mTangents[i].y;
            vector[2] = mesh->mTangents[i].z;
            vertex.Tangent = vector;

            // bitangent
            vector[0] = mesh->mBitangents[i].x;
            vector[1] = mesh->mBitangents[i].y;
            vector[2] = mesh->mBitangents[i].z;
            vertex.Bitangent = vector;
        }
        else
            vertex.TexCoords = QVector2D(0, 0);

        vertices.push_back(vertex);
    }

    // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
    //加载VEO
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        // retrieve all indices of the face and store them in the indices vector
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    QVector<Texture*> textures;

    //处理材质
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

    // 1. 漫反射贴图
    vector<Texture*> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
    for(auto &it:diffuseMaps){
        textures.push_back(it);
    }
    // 2. 镜面反射
    vector<Texture*> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
    for(auto &it:specularMaps){
        textures.push_back(it);
    }
    // 3. 法向量贴图
    std::vector<Texture*> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
    for(auto &it:normalMaps){
        textures.push_back(it);
    }
    // 4. 高度图
    std::vector<Texture*> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
    for(auto &it:heightMaps){
        textures.push_back(it);
    }

    // return a mesh object created from the extracted mesh data
    return new Mesh(vertices, indices,textures);
}

vector<Texture*> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, QString typeName)
{
    vector<Texture*> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;//Texture的地址
        mat->GetTexture(type, i, &str);
        // 检查Texture是否已经载入
        bool skip = false;
        for (int j = 0; j < textures_loaded.size(); j++)
        {
            if (std::strcmp(textures_loaded[j]->path.toStdString().c_str(), str.C_Str()) == 0)
            {
                textures.push_back(textures_loaded[j]);
                skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                break;
            }
        }
        if (!skip)
        {   // if texture hasn't been loaded already, load it
            Texture* texture = new Texture;
            QImage data(directory.filePath(str.C_Str()));
            if(!data.isNull()){
                texture->texture.setData(data);
                texture->type = typeName;
                texture->path = str.C_Str();
                textures.push_back(texture);//在该容器中存储的只有id，类型和路径，对应特定的网络mesh
                textures_loaded.push_back(texture);// 将其存储为整个模型加载的纹理，以确保我们不会不必要地加载重复的纹理。
            }
            else{
                qDebug()<<"未能成功加载纹理："<<directory.filePath(str.C_Str());
            }
        }
    }
    return textures;
}

void Model::loadTriangle()
{
    for(int i=0;i<meshes.size();++i){
        Mesh* mesh = meshes[i];
        for(int j=0;j<mesh->indices.size();j+=3){
            QVector3D v[3];
            QVector3D Color(0.2,0.3,0.5);
            v[0] = mesh->vertices.at(mesh->indices.at(j)).Position;
            v[1] = mesh->vertices.at(mesh->indices.at(j+1)).Position;
            v[2] = mesh->vertices.at(mesh->indices.at(j+2)).Position;
            Triangle* tri = new Triangle(v,Color);
            triangles.push_back(tri);
        }
    }
}

void Model::Draw(QOpenGLShaderProgram &shader)
{
//    for(int i=0;i<triangles.size();++i){
//        triangles.at(i)->Draw(shader);
//    }
    for (unsigned int i = 0; i < meshes.size(); i++)
        meshes[i]->Draw(shader);
}

QVector3D Model::getlightpos()
{
    return model.getmodel()*QVector3D(0,1,0);
}

QVector3D Model::getlightNormal()
{
    return QVector3D(0,0,0);//为点光源
}

float Model::getArea()
{
    return 0;
}






