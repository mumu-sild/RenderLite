#ifndef BVH_H
#define BVH_H


#include <vector>
#include "Object.h"
#include "ray.h"
#include "Bounds3.h"
#include "Intersection.h"
#include "vec.h"

struct BVHBuildNode;
class BVHAccel;


struct BVHBuildNode {
    Bounds3 bounds;
    BVHBuildNode *left;
    BVHBuildNode *right;
    Object* object;
    float area;

public:
    //int splitAxis=0, firstPrimOffset=0, nPrimitives=0;

    // BVHBuildNode Public Methods
    BVHBuildNode(){
        bounds = Bounds3();
        left = nullptr;right = nullptr;
        object = nullptr;
    }
};

// BVHAccel Declarations
inline int leafNodes, totalLeafNodes, totalPrimitives, interiorNodes;

class BVHAccel {
public:
    //
    enum class SplitMethod { NAIVE, SAH };

    const int maxPrimsInNode;
    const SplitMethod splitMethod;
    std::vector<Object*> objectlist;


public:
    // BVHAccel Public Methods
    BVHAccel(std::vector<Object*> p, int maxPrimsInNode = 1, SplitMethod splitMethod = SplitMethod::NAIVE);

    ~BVHAccel();

    BVHBuildNode* recursiveBuild(std::vector<Object*>objects);

    //Intersection Intersect(const Ray &ray) const;
    //Intersection getIntersection(BVHBuildNode* node, const Ray& ray)const;
    bool IntersectP(const Ray &ray) const;
    BVHBuildNode* root;
    Bounds3 WorldBound() const;
    // BVHAccel Private Methods


    void getSample(BVHBuildNode* node, float p, Intersection &pos, float &pdf);
    void Sample(Intersection &pos, float &pdf);
};



#endif // BVH_H
