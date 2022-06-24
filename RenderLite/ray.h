#ifndef RAY_H
#define RAY_H

#include "vec.h"

class Ray
{
public:
    Ray() {};
    Ray(const point3& origin, const vec3 direction) :orig(origin), dir(direction) {
        dir_inv = vec3(1./direction.x(), 1./direction.y(), 1./direction.z());
    };
    ~Ray();

private:
    point3 orig;
    vec3 dir;
public:
    vec3 dir_inv;//  1/dir

public:
    point3 origin() const { return orig; }
    vec3 direction() const { return dir; }
    point3 at(double t) const {
        return orig + t * dir;
    }
};



Ray::~Ray()
{
}

#endif // RAY_H
