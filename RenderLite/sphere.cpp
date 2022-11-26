#include "sphere.h"
#include "Global.h"

bool Sphere::isinitial = false;
QVector<Vertex> Sphere::v;
QVector<unsigned int> Sphere::indices;
unsigned int Sphere::indexCount;

Sphere::Sphere()
    :VBO(QOpenGLBuffer::VertexBuffer),EBO(QOpenGLBuffer::IndexBuffer)
{
    initializeOpenGLFunctions();
    if(!isinitial){
        const unsigned int X_SEGMENTS = 64;
        const unsigned int Y_SEGMENTS = 64;
        for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
        {
            for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
            {
                float xSegment = (float)x / (float)X_SEGMENTS;//x-z轴平面的角度
                float ySegment = (float)y / (float)Y_SEGMENTS;//高度角,最高处为0，最低处为1
                float xPos = cos(xSegment * 2.0f * M_PI) * sin(ySegment * M_PI);
                float yPos = cos(ySegment * M_PI);
                float zPos = sin(xSegment * 2.0f * M_PI) * sin(ySegment * M_PI);

                Vertex v_;
                v_.Position = QVector3D(xPos,yPos,zPos);
                v_.TexCoords = QVector2D(xSegment,ySegment);
                v_.Normal = QVector3D(xPos,yPos,zPos);
                v.push_back(v_);
            }
        }
        bool oddRow = false;
        for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
        {
            if (!oddRow) // even rows: y == 0, y == 2; and so on
            {
                for (unsigned int x = 0; x <= X_SEGMENTS; ++x)//y为偶数。x in (0,X_SEGMENTS)
                {
                    indices.push_back(y       * (X_SEGMENTS + 1) + x);//第y维度的经度x
                    indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);//第y+1维度的经度x
                }
            }
            else
            {
                for (int x = X_SEGMENTS; x >= 0; --x)//y为奇数
                {
                    indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                    indices.push_back(y       * (X_SEGMENTS + 1) + x);
                }
            }
            oddRow = !oddRow;
        }
        indexCount = static_cast<unsigned int>(indices.size());
        isinitial = true;
    }
    setupVAO();
}

void Sphere::Draw(QOpenGLShaderProgram &shader)
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
    glDrawElements(GL_TRIANGLE_STRIP, indices.size(), GL_UNSIGNED_INT, 0);
    VAO.release();
}

QVector3D Sphere::getlightpos()
{
    return model.getmodel() * QVector3D(0,0,0);
}

QVector3D Sphere::getlightNormal()
{
    return QVector3D(0,0,0);//为点光源
}

float Sphere::getArea()
{
    return pow(model.getScale().x(),2)*4*M_PI;
}

Sphere::~Sphere()
{
    VAO.destroy();
    VBO.destroy();
    EBO.destroy();
}


void Sphere::setupVAO()
{
    VAO.create();
    VAO.bind();

    VBO.create();
    VBO.bind();
    VBO.allocate(v.data(),sizeof (Vertex)*v.size());

    EBO.create();
    EBO.bind();
    EBO.allocate(indices.data(),sizeof(unsigned int)*indices.size());

    // vertex Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

    //vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

    VAO.release();
}
