#ifndef RAY_H
#define RAY_H

#include "vec.h"

class Ray
{
public:
    Ray() {};
    Ray(const point3& origin, const vec3 direction) :orig(origin), dir(direction) {	};
    ~Ray();

private:
    point3 orig;
    vec3 dir;

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
