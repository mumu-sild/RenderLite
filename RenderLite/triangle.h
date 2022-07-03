#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <QVector3D>
#include <QVector>
#include <QOpenGLTexture>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>

#include "Object.h"
#include "Intersection.h"
#include "Bounds3.h"
#include "Global.h"
#include "Texture.h"
#include "Vertex.h"


class Triangle:public Object,public QOpenGLFunctions
{
private:
    QVector<Vertex> v;
    //QVector3D v[3];
    //QVector2D texture[3];
    //QVector3D normal;

    QVector3D edge0_1,edge0_2;
    QVector3D center;
    QVector<Texture*> textures;

    float area;
    //Material* m;
private:
    QOpenGLVertexArrayObject VAO;
    QOpenGLBuffer VBO;
public:
    Triangle();
    Triangle(QVector3D _v[3],QVector3D Color);
    //Triangle(QVector3D _v[3],QVector2D texcoords[3],QVector<Texture*> texture);
//    bool intersect(const Ray& ray)override;//û��ʵ��
//    bool intersect(const Ray& ray, float& tnear,uint32_t& index)
//                        const override;//û��ʵ��
//    Intersection getIntersection(Ray ray) override;
//    Bounds3 getBounds() override;
//    float getArea() override {return area;}
//    //�������ֵ�õ������������һ�㣬���õ��õ�pdf���õ�ĸ����ܶȣ�Ϊ1/�����������
//    void Sample(Intersection &pos, float &pdf)override;

    //void setTexture(QVector2D* texcoords,QVector<Texture*> texture);
    void Draw(QOpenGLShaderProgram& shader)override;
    QVector3D getlightpos()override;
    QVector3D getlightNormal()override;
    ~Triangle();
private:
    void setupVAO();

};


#endif // TRIANGLE_H
