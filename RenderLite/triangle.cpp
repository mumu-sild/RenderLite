#include "triangle.h"

Triangle::Triangle(QVector3D _v[3],QVector3D Color)
    :VBO(QOpenGLBuffer::VertexBuffer)
{
    initializeOpenGLFunctions();
    edge0_1 = _v[1] - _v[0];
    edge0_2 = _v[2] - _v[0];
    QVector3D normal = QVector3D::crossProduct(edge0_1, edge0_2).normalized();
    area = QVector3D::crossProduct(edge0_1, edge0_2).length()*0.5;
    color = Color;

    Vertex vertex;
    for(int i=0;i<3;i++){
        vertex.Position = _v[i];
        vertex.Normal = normal;
        v.push_back(vertex);
    }

    setupVAO();
}

//Triangle::Triangle(QVector3D _v[], QVector2D texcoords[], QVector<Texture *> texture)
//{

//}

//void Triangle::setTexture(QVector2D *texcoords, QVector<Texture *> texture)
//{

//}

void Triangle::setupVAO()
{
    VAO.create();
    VAO.bind();

    VBO.create();
    VBO.bind();
    VBO.allocate(v.data(),sizeof (Vertex)*v.size());

    // vertex Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

    //vertex texture coords
    if(!textures.empty()){
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    }

    VAO.release();
}

void Triangle::Draw(QOpenGLShaderProgram &shader)
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
    glDrawArrays(GL_TRIANGLES,0,3);
    VAO.release();
}

QVector3D Triangle::getlightpos()
{
    QVector3D center = (v.at(0).Position+v.at(1).Position+v.at(2).Position)/3;
    QVector3D point = QVector3D(model.getmodel()*QVector4D(center,1.0f));
    qDebug()<<model.getmodel();
    qDebug()<<point;

    return point;
}

//bool Triangle::intersect(const Ray &ray)
//{
//    return true;
//}

//bool Triangle::intersect(const Ray &ray, float &tnear, uint32_t &index) const
//{
//    return false;
//}

//Intersection Triangle::getIntersection(Ray ray)
//{
//    Intersection inter;
//    //如果光线照射三角形背面，返回没有碰撞
//    if(QVector3D::dotProduct(ray.direction(),normal)>0){
//        return inter;
//    }
//    double u,v,t;
//    QVector3D S0 = ray.origin()-v0;//tvec
//    QVector3D S1 = QVector3D::crossProduct(ray.direction(),edge0_2);//pvec
//    QVector3D S2 = QVector3D::crossProduct(S0, edge0_1);//qvec
//    double denominator = QVector3D::dotProduct(edge0_1,S1);

//    //如果分母太小，返回没有碰撞
//    if(fabs(denominator)<EPSILON) return inter;

//    double denominator_inv = 1. /denominator;

//    //o+t*dir = (1-u-v)v0 + u*v1 + v*v2;
//    //计算t,u,v
//    u = QVector3D::dotProduct(S0, S1) * denominator_inv;
//    if (u < 0 || u > 1) return inter;

//    v = QVector3D::dotProduct(ray.direction(), S2) * denominator_inv;
//    if (v < 0 || u + v > 1) return inter;

//    t = QVector3D::dotProduct(edge0_2, S2) * denominator_inv;
//    if (t < 0)  return inter;

//    //返回相交信息
//    inter.happened = true;
//    inter.distance = t;
//    inter.coords = ray.at(t);
//    //inter.m = m;
//    inter.normal = normal;
//    inter.obj = this;
//    return inter;
//}

//Bounds3 Triangle::getBounds()
//{
//    return Union(Bounds3(v0, v1), v2);
//}

//void Triangle::Sample(Intersection &pos, float &pdf){
//    float x = std::sqrt(random_float()), y = random_float();
//    pos.coords = v0 * (1.0f - x) + v1 * (x * (1.0f - y)) + v2 * (x * y);
//    pos.normal = this->normal;
//    pdf = 1.0f / area;
//}

Triangle::~Triangle()
{

}


