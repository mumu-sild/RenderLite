#ifndef BOUNDS3_H
#define BOUNDS3_H


#include <limits>
#include <array>

#include "ray.h"
#include "vec.h"
#include "Global.h"

/**
 * Bounds3类
 * 确定一个三维空间包围盒
 *
 *
*/
class Bounds3
{
  public:
    vec3 pMin, pMax; //两个点来确定包围盒
    Bounds3()//默认构造函数，包围盒默认为double范围内最大包围盒
    {
        double minNum = std::numeric_limits<double>::lowest();
        double maxNum = std::numeric_limits<double>::max();
        pMax = vec3(minNum, minNum, minNum);
        pMin = vec3(maxNum, maxNum, maxNum);
    }
    Bounds3(const vec3 p) : pMin(p), pMax(p) {}
    Bounds3(const vec3 p1, const vec3 p2)
    {
        pMin = vec3(min(p1.x(), p2.x()), min(p1.y(), p2.y()), min(p1.z(), p2.z()));
        pMax = vec3(max(p1.x(), p2.x()), max(p1.y(), p2.y()), max(p1.z(), p2.z()));
    }

    //对角线向量
    vec3 Diagonal() const { return pMax - pMin; }

    //获取包围盒最大轴方向,0--x,1--y,2--z
    int maxExtent() const
    {
        vec3 d = Diagonal();
        if (d.x() > d.y() && d.x() > d.z())
            return 0;
        else if (d.y() > d.z())
            return 1;
        else
            return 2;
    }

    //包围盒表面积
    double SurfaceArea() const
    {
        vec3 d = Diagonal();
        return 2 * (d.x() * d.y() + d.x() * d.z() + d.y() * d.z());
    }

    //包围盒中心坐标
    vec3 Centroid() { return 0.5 * pMin + 0.5 * pMax; }

    //返回 两包围盒 相交构成的 包围盒
    Bounds3 Intersect(const Bounds3& b)
    {
        return Bounds3(vec3(fmax(pMin.x(), b.pMin.x()), fmax(pMin.y(), b.pMin.y()),
                                fmax(pMin.z(), b.pMin.z())),
                       vec3(fmin(pMax.x(), b.pMax.x()), fmin(pMax.y(), b.pMax.y()),
                                fmin(pMax.z(), b.pMax.z())));
    }

    //p点在包围盒内部的比值坐标（没有判断是否在包围盒外部）
    vec3 Offset(const vec3& p) const
    {
        vec3 o = p - pMin;
        if (pMax.x() > pMin.x())    o[0] /= pMax.x() - pMin.x();
        if (pMax.y() > pMin.y())    o[1] /= pMax.y() - pMin.y();
        if (pMax.z() > pMin.z())    o[2] /= pMax.z() - pMin.z();
        return o;
    }

    //判断两个包围盒是否相交
    bool Overlaps(const Bounds3& b1, const Bounds3& b2)
    {
        bool x = (b1.pMax.x() >= b2.pMin.x()) && (b1.pMin.x() <= b2.pMax.x());
        bool y = (b1.pMax.y() >= b2.pMin.y()) && (b1.pMin.y() <= b2.pMax.y());
        bool z = (b1.pMax.z() >= b2.pMin.z()) && (b1.pMin.z() <= b2.pMax.z());
        return (x && y && z);
    }

    //判断p点是否在b包围盒中
    bool Inside(const vec3& p, const Bounds3& b)
    {
        return (p.x() >= b.pMin.x() && p.x() <= b.pMax.x() && p.y() >= b.pMin.y() &&
                p.y() <= b.pMax.y() && p.z() >= b.pMin.z() && p.z() <= b.pMax.z());
    }

    //无用
    inline const vec3& operator[](int i) const
    {
        return (i == 0) ? pMin : pMax;
    }

    /*
     * 核心功能：判断光线Ray和该包围盒是否相交
     * invDir是ray的逆，为了方便除法运算。
     * dirisNeg是判断光线传播方向为正为负。
     */
    inline bool IntersectP(const Ray& ray, const vec3& invDir,
                           const std::array<int, 3>& dirisNeg) const;
};

/*
 * 核心功能：判断光线Ray和该包围盒是否相交
 * invDir是ray的逆，为了方便除法运算。
 * dirisNeg是判断光线传播方向为正为负。
 */
inline bool Bounds3::IntersectP(const Ray& ray, const vec3& invDir,
                                const std::array<int, 3>& dirIsNeg) const
{
    // invDir: ray direction(x,y,z), invDir=(1.0/x,1.0/y,1.0/z), use this because Multiply is faster that Division
    // dirIsNeg: ray direction(x,y,z), dirIsNeg=[int(x>0),int(y>0),int(z>0)], use this to simplify your logic
    // TODO test if ray bound intersects
    float tEnter = -std::numeric_limits<float>::infinity();//求进入点的最大值
    float tExit = std::numeric_limits<float>::infinity();//求离开点的最小值
    for (int i = 0; i < 3; i++)
    {
        float min = (pMin[i] - ray.origin()[i]) * invDir[i];//循环求x，y，z轴的tmin
        float max = (pMax[i] - ray.origin()[i]) * invDir[i];//tmax
        //如果光线传播方向为负方向，则pmax为进入轴，pmin为离开轴。交换min，max即可
        if (!dirIsNeg[i])
        {
            std::swap(min, max);
        }
        tEnter = std::max(min, tEnter);
        tExit = std::min(max, tExit);
    }
    return tEnter <= tExit&& tExit >= 0;//一个=号决定成像？？？？
}

//包围两个包围盒的 包围盒
inline Bounds3 Union(const Bounds3& b1, const Bounds3& b2)
{
    Bounds3 ret;
    ret.pMin = vec3::Min(b1.pMin, b2.pMin);
    ret.pMax = vec3::Max(b1.pMax, b2.pMax);
    return ret;
}

//包围包围盒和p点的 包围盒
inline Bounds3 Union(const Bounds3& b, const vec3& p)
{
    Bounds3 ret;
    ret.pMin = vec3::Min(b.pMin, p);
    ret.pMax = vec3::Max(b.pMax, p);
    return ret;
}

#endif // BOUNDS3_H
