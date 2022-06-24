#ifndef INTERSECTION_H
#define INTERSECTION_H

#include <limits>

#include "vec.h"
//#include "Material.hpp"

class Object;


class Intersection
{
    Intersection(){
        happened=false;
        coords=vec3();
        normal=vec3();
        distance= std::numeric_limits<double>::max();
        obj =nullptr;
        //m=nullptr;
    }
    bool happened;//�Ƿ�����ײ
    vec3 coords;//
    vec3 tcoords;
    vec3 normal; //normal
    //vec3 emit;
    double distance;
    Object* obj;//����
    //Material* m;
};

#endif // INTERSECTION_H
