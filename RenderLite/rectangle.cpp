#include "rectangle.h"

rectangle::rectangle(float width, float height, QVector3D color)
    :width(width),height(height),color(color),
     VBO(QOpenGLBuffer::VertexBuffer),
     EBO(QOpenGLBuffer::IndexBuffer)
{
    initializeOpenGLFunctions();
    Vertex vertex[4];
    vertex[0].Position = QVector3D(-width/2,0,-height/2);
    vertex[1].Position = QVector3D( width/2,0,-height/2);
    vertex[2].Position = QVector3D( width/2,0, height/2);
    vertex[3].Position = QVector3D(-width/2,0, height/2);
    vertex[0].TexCoords = QVector2D(0,0);
    vertex[1].TexCoords = QVector2D(1,0);
    vertex[2].TexCoords = QVector2D(1,1);
    vertex[3].TexCoords = QVector2D(0,1);
    for(int i=0;i<4;i++){
        vertex[i].Normal = QVector3D(0,1,0);
        v.push_back(vertex[i]);
    }
    unsigned int pointindex[6] = {0,3,2,0,2,1};
    for(int i=0;i<6;i++){
        indices.push_back(pointindex[i]);
    }

    setupVAO();
}

void rectangle::Draw(QOpenGLShaderProgram &shader)
{
    if(textures.empty()){
        shader.setUniformValue("Color",color);
    }
    else{
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
            shader.setUniformValue((name + number).toStdString().c_str(),i);
            // and finally bind the texture
        }
    }
    VAO.bind();
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    VAO.release();
}

QVector3D rectangle::getlightpos()
{
    return model.getmodel()*QVector3D(0,0,0);
}

QVector3D rectangle::getlightNormal()
{
    QMatrix4x4 modle_inv = model.getmodel().inverted().transposed();
    QVector3D transNormal = QVector3D(modle_inv*QVector4D(v[0].Normal,0.0f));
    return transNormal;
}

float rectangle::getArea()
{
    return width*height;
}


void rectangle::setupVAO()
{
    VAO.create();
    VAO.bind();

    VBO.create();
    VBO.bind();
    VBO.allocate(v.data(),sizeof (Vertex)*v.size());

    EBO.create();
    EBO.bind();
    EBO.allocate(indices.data(),sizeof(unsigned int)*indices.size());

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

    VAO.release();
}

rectangle::~rectangle()
{
    VAO.destroy();
    VBO.destroy();
    EBO.destroy();
}

