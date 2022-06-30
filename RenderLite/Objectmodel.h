#ifndef OBJECTMODEL_H
#define OBJECTMODEL_H

#include <QMatrix4x4>
#include "Global.h"

class Objectmodel{
private:
    QMatrix4x4 model;
    QVector3D mposition;
    QVector3D mrotate;
    QVector3D mscale;

public:
    Objectmodel(){mscale=QVector3D(1,1,1);};

    void translate(QVector3D xyz);
    void rotate(int xyz,double offset);//0-x.1-y,2-z
    void scale(QVector3D mul);

    void setPosition(QVector3D xyz);
    void setRotate(QVector3D xyz);
    void setScale(QVector3D xyz);
    QVector3D getPosition();
    QVector3D getRotate();
    QVector3D getScale();

    QMatrix4x4& getmodel();
    void updateModel();

};

inline void Objectmodel::translate(QVector3D xyz)
{
    mposition += xyz;
}

inline void Objectmodel::rotate(int xyz, double offset)
{
    if(xyz==0)mrotate[0]+=offset;
    else if (xyz==1)mrotate[1]+=offset;
    else if (xyz==2)mrotate[2]+=offset;
}

inline void Objectmodel::scale(QVector3D mul)
{
    mscale*=mul;
}

inline void Objectmodel::setPosition(QVector3D xyz)
{
    mposition=xyz;
}

inline void Objectmodel::setRotate(QVector3D xyz)
{
    mrotate = xyz;
}

inline void Objectmodel::setScale(QVector3D xyz)
{
    mscale = xyz;
}

inline QVector3D Objectmodel::getPosition()
{
    return mposition;
}

inline QVector3D Objectmodel::getRotate()
{
    return mrotate;
}

inline QVector3D Objectmodel::getScale()
{
    return mscale;
}

inline QMatrix4x4 &Objectmodel::getmodel()
{
    updateModel();
    return model;
}

inline void Objectmodel::updateModel()
{
    model.setToIdentity();
    model.scale(mscale);
    model.rotate(mrotate.x(),QVector3D(1,0,0));
    model.rotate(mrotate.y(),QVector3D(0,1,0));
    model.rotate(mrotate.z(),QVector3D(0,0,1));
    model.translate(mposition);
}

#endif // OBJECTMODEL_H
