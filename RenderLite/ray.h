#ifndef RAY_H
#define RAY_H

#include <QVector3D>


class Ray
{


public:
    Ray() {};
    Ray(const QVector3D& origin, const QVector3D direction) :orig(origin), dir(direction) {
        dir_inv = QVector3D(1./direction.x(), 1./direction.y(), 1./direction.z());
    };
    ~Ray(){};

private:
    QVector3D orig;
    QVector3D dir;
public:
    QVector3D dir_inv;//  1/dir

public:
    QVector3D origin() const { return orig; }
    QVector3D direction() const { return dir; }
    QVector3D at(double t) const {
        return orig + t * dir;
    }
};

#endif // RAY_H
