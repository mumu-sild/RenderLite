#include <algorithm>
#include <cassert>
#include <vector>

#include "BVH.h"
//不能加入Global？

extern double random_double();


/*
 * 构建传入的Object的BVH数据结构
 * maxPrimsInNode:
 * splitMethod:
 */
BVHAccel::BVHAccel(std::vector<Object*> p, int maxPrimsInNode,
                   SplitMethod splitMethod)
    : maxPrimsInNode(std::min(255, maxPrimsInNode)), splitMethod(splitMethod),
      objectlist(std::move(p))
{
    //objectlist = std::move(p);move将右值转化为左值
    time_t start, stop;
    time(&start);

    if (objectlist.empty())return;

    root = recursiveBuild(objectlist);

    time(&stop);
    double diff = difftime(stop, start);
    int hrs = (int)diff / 3600;
    int mins = ((int)diff / 60) - (hrs * 60);
    int secs = (int)diff - (hrs * 3600) - (mins * 60);

    printf(
        "\rBVH Generation complete: \nTime Taken: %i hrs, %i mins, %i secs\n\n",
        hrs, mins, secs);
}

/*
 * 递归构建BVH树
 * 只对Object进行分类，而不对
 */
BVHBuildNode* BVHAccel::recursiveBuild(std::vector<Object*> objects)
{
    BVHBuildNode* node = new BVHBuildNode();

    // Compute bounds of all primitives in BVH node
    Bounds3 bounds;
    for (int i = 0; i < objects.size(); ++i)
        bounds = Union(bounds, objects[i]->getBounds());
    if (objects.size() == 1) {
        // Create leaf _BVHBuildNode_
        node->bounds = objects[0]->getBounds();
        node->object = objects[0];
        node->left = nullptr;
        node->right = nullptr;
        node->area = objects[0]->getArea();
        return node;
    }
    else if (objects.size() == 2) {
        node->left = recursiveBuild(std::vector{objects[0]});
        node->right = recursiveBuild(std::vector{objects[1]});

        node->bounds = Union(node->left->bounds, node->right->bounds);
        node->area = node->left->area + node->right->area;
        return node;
    }
    else {
        Bounds3 centroidBounds;
        for (int i = 0; i < objects.size(); ++i)
            centroidBounds =
                Union(centroidBounds, objects[i]->getBounds().Centroid());
        int dim = centroidBounds.maxExtent();//获取最大轴
        switch (dim) {
        case 0:
            std::sort(objects.begin(), objects.end(), [](auto f1, auto f2) {
                return f1->getBounds().Centroid().x() < \
                       f2->getBounds().Centroid().x();
            });
            break;
        case 1:
            std::sort(objects.begin(), objects.end(), [](auto f1, auto f2) {
                return f1->getBounds().Centroid().y() < \
                       f2->getBounds().Centroid().y();
            });
            break;
        case 2:
            std::sort(objects.begin(), objects.end(), [](auto f1, auto f2) {
                return f1->getBounds().Centroid().z() < \
                       f2->getBounds().Centroid().z();
            });
            break;
        }

        auto beginning = objects.begin();
        auto middling = objects.begin() + (objects.size() / 2);
        auto ending = objects.end();

        auto leftshapes = std::vector<Object*>(beginning, middling);
        auto rightshapes = std::vector<Object*>(middling, ending);

        assert(objects.size() == (leftshapes.size() + rightshapes.size()));

        node->left = recursiveBuild(leftshapes);
        node->right = recursiveBuild(rightshapes);

        node->bounds = Union(node->left->bounds, node->right->bounds);
        node->area = node->left->area + node->right->area;
    }

    return node;
}

/*

Intersection BVHAccel::Intersect(const Ray& ray) const
{
    Intersection isect;
    if (!root)
        return isect;
    isect = BVHAccel::getIntersection(root, ray);
    return isect;
}
*/

/*
Intersection BVHAccel::getIntersection(BVHBuildNode* node, const Ray& ray) const
{
    // TODO Traverse the BVH to find intersection
    Intersection inter;
    //光线方向
    float x = ray.direction().x();
    float y = ray.direction().y();
    float z = ray.direction().z();

    //传参
    //在IntersectP函数中说明了dirIsNeg=[int(x>0),int(y>0),int(z>0)]
    std::array<int, 3> dirIsNeg = { int(x > 0),int(y > 0),int(z > 0) };
    if (node->bounds.IntersectP(ray, ray.dir_inv, dirIsNeg)) {//如果和包围盒碰撞
        if (node->left == nullptr && node->right == nullptr) {//如果没有子包围盒
            inter = node->object->getIntersection(ray);//判断光线是否与包围盒内物体相交，返回相交点信息
            return inter;
        }
        else {//递归调用,构建BVH树时保证了一个结点要不有左右子树，要左右子树都没有。
            Intersection hitleft = getIntersection(node->left, ray);
            Intersection hitright = getIntersection(node->right, ray);
            if (hitleft.distance < hitright.distance) {//没有碰撞返回fmax
                return hitleft;
            }
            else {
                return hitright;
            }
        }
    }
    //如果没有和包围盒碰撞，直接返回inter
    return inter;
}
*/


//根据p值找到一个三角形，得到该三角形的Sample
void BVHAccel::getSample(BVHBuildNode* node, float p, Intersection &pos, float &pdf){
    if(node->left == nullptr || node->right == nullptr){
        node->object->Sample(pos, pdf);
        pdf *= node->area;//????pdf==1？？？
        return;
    }
    if(p < node->left->area) getSample(node->left, p, pos, pdf);
    else getSample(node->right, p - node->left->area, pos, pdf);
}

void BVHAccel::Sample(Intersection &pos, float &pdf){
    float p = std::sqrt(random_double()) * root->area;
    getSample(root, p, pos, pdf);
    pdf /= root->area;
}
