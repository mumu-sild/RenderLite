#include "Mesh.h"

Mesh::Mesh(QVector<Vertex> vertices, QVector<unsigned int> indices, QVector<Texture*> textures)
    :VEO(QOpenGLBuffer::IndexBuffer),
     VBO(QOpenGLBuffer::VertexBuffer)
{
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;
    initializeOpenGLFunctions();

    // now that we have all the required data, set the vertex buffers and its attribute pointers.
    setupMesh();
}



void Mesh::Draw(QOpenGLShaderProgram& shader)
{
    // bind appropriate textures
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr = 1;
    unsigned int heightNr = 1;
    for (int i = 0; i < textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
        // retrieve texture number (the N in diffuse_textureN)
        QString number;
        QString name = textures[i]->type;
        if (name == "texture_diffuse")
            number.setNum(diffuseNr++);
        else if (name == "texture_specular")
            number.setNum(specularNr++); // transfer unsigned int to stream
        else if (name == "texture_normal")
            number.setNum(normalNr++); // transfer unsigned int to stream
        else if (name == "texture_height")
            number.setNum(heightNr++); // transfer unsigned int to stream

        //设置GPU中（name+number）的字符串与i号结点相连
        textures[i]->texture.bind();//glBindTexture(GL_TEXTURE_2D, textures[i]->id);
        shader.setUniformValue(("material." + name + number).toStdString().c_str(),i);
        // and finally bind the texture

    }


    // draw mesh

    VAO.bind();
    //shader.setUniformValue("model",model);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    VAO.release();

}

Mesh::~Mesh()
{
    VAO.destroy();
    VBO.destroy();
    VEO.destroy();
}

void Mesh::setupMesh()
{
    // create buffers/arrays
    VAO.create();
    VAO.bind();

    VBO.create();
    VBO.bind();
    VBO.allocate(vertices.data(),sizeof (Vertex)*vertices.size());

    VEO.create();
    VEO.bind();
    VEO.allocate(indices.data(),sizeof(unsigned int)*indices.size());


    // set the vertex attribute pointers
    // vertex Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    // vertex tangent
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
    // vertex bitangent
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
    // ids
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_INT,  GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));

    // weights
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));
    VAO.release();
}
