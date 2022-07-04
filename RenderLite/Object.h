#ifndef OBJECT_H
#define OBJECT_H

#include <QOpenGLShaderProgram>
#include <QMatrix4x4>


#include "Bounds3.h"
#include "ray.h"
#include "Intersection.h"
#include "Objectmodel.h"
#include "Setting.h"


class Object
{
public:
    Objectmodel model;
    bool islight = false;
    QVector3D color = ObjectDefaultColor;
public:
    Object(){};

    virtual void Draw(QOpenGLShaderProgram& shader)=0;

    virtual QVector3D getlightpos()=0;
    virtual QVector3D getlightNormal()=0;
    virtual ~Object(){};
    //virtual bool intersect(const Ray& ray) = 0;
    //virtual bool intersect(const Ray& ray, float &, uint32_t &) const = 0;
    //virtual Intersection getIntersection(Ray _ray) = 0;
    //virtual void getSurfaceProperties(const vec3 &, const vec3 &, const uint32_t &, const vec2 &, vec3 &, vec2 &) const = 0;
    //virtual vec3 evalDiffuseColor(const vec2 &) const =0;
    //virtual Bounds3 getBounds()=0;//计算该物体的Bounds;
    //virtual float getArea()=0;
    //virtual void Sample(Intersection &pos, float &pdf)=0;
    //virtual bool hasEmit()=0;
};


#endif // OBJECT_H
