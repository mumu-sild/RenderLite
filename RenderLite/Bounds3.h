#ifndef BOUNDS3_H
#define BOUNDS3_H


#include <limits>
#include <array>

#include "ray.h"
#include "vec.h"
#include "Global.h"

/**
 * Bounds3��
 * ȷ��һ����ά�ռ��Χ��
 *
 *
*/
class Bounds3
{
  public:
    vec3 pMin, pMax; //��������ȷ����Χ��
    Bounds3()//Ĭ�Ϲ��캯������Χ��Ĭ��Ϊdouble��Χ������Χ��
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

    //�Խ�������
    vec3 Diagonal() const { return pMax - pMin; }

    //��ȡ��Χ������᷽��,0--x,1--y,2--z
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

    //��Χ�б����
    double SurfaceArea() const
    {
        vec3 d = Diagonal();
        return 2 * (d.x() * d.y() + d.x() * d.z() + d.y() * d.z());
    }

    //��Χ����������
    vec3 Centroid() { return 0.5 * pMin + 0.5 * pMax; }

    //���� ����Χ�� �ཻ���ɵ� ��Χ��
    Bounds3 Intersect(const Bounds3& b)
    {
        return Bounds3(vec3(fmax(pMin.x(), b.pMin.x()), fmax(pMin.y(), b.pMin.y()),
                                fmax(pMin.z(), b.pMin.z())),
                       vec3(fmin(pMax.x(), b.pMax.x()), fmin(pMax.y(), b.pMax.y()),
                                fmin(pMax.z(), b.pMax.z())));
    }

    //p���ڰ�Χ���ڲ��ı�ֵ���꣨û���ж��Ƿ��ڰ�Χ���ⲿ��
    vec3 Offset(const vec3& p) const
    {
        vec3 o = p - pMin;
        if (pMax.x() > pMin.x())    o[0] /= pMax.x() - pMin.x();
        if (pMax.y() > pMin.y())    o[1] /= pMax.y() - pMin.y();
        if (pMax.z() > pMin.z())    o[2] /= pMax.z() - pMin.z();
        return o;
    }

    //�ж�������Χ���Ƿ��ཻ
    bool Overlaps(const Bounds3& b1, const Bounds3& b2)
    {
        bool x = (b1.pMax.x() >= b2.pMin.x()) && (b1.pMin.x() <= b2.pMax.x());
        bool y = (b1.pMax.y() >= b2.pMin.y()) && (b1.pMin.y() <= b2.pMax.y());
        bool z = (b1.pMax.z() >= b2.pMin.z()) && (b1.pMin.z() <= b2.pMax.z());
        return (x && y && z);
    }

    //�ж�p���Ƿ���b��Χ����
    bool Inside(const vec3& p, const Bounds3& b)
    {
        return (p.x() >= b.pMin.x() && p.x() <= b.pMax.x() && p.y() >= b.pMin.y() &&
                p.y() <= b.pMax.y() && p.z() >= b.pMin.z() && p.z() <= b.pMax.z());
    }

    //����
    inline const vec3& operator[](int i) const
    {
        return (i == 0) ? pMin : pMax;
    }

    /*
     * ���Ĺ��ܣ��жϹ���Ray�͸ð�Χ���Ƿ��ཻ
     * invDir��ray���棬Ϊ�˷���������㡣
     * dirisNeg���жϹ��ߴ�������Ϊ��Ϊ����
     */
    inline bool IntersectP(const Ray& ray, const vec3& invDir,
                           const std::array<int, 3>& dirisNeg) const;
};

/*
 * ���Ĺ��ܣ��жϹ���Ray�͸ð�Χ���Ƿ��ཻ
 * invDir��ray���棬Ϊ�˷���������㡣
 * dirisNeg���жϹ��ߴ�������Ϊ��Ϊ����
 */
inline bool Bounds3::IntersectP(const Ray& ray, const vec3& invDir,
                                const std::array<int, 3>& dirIsNeg) const
{
    // invDir: ray direction(x,y,z), invDir=(1.0/x,1.0/y,1.0/z), use this because Multiply is faster that Division
    // dirIsNeg: ray direction(x,y,z), dirIsNeg=[int(x>0),int(y>0),int(z>0)], use this to simplify your logic
    // TODO test if ray bound intersects
    float tEnter = -std::numeric_limits<float>::infinity();//����������ֵ
    float tExit = std::numeric_limits<float>::infinity();//���뿪�����Сֵ
    for (int i = 0; i < 3; i++)
    {
        float min = (pMin[i] - ray.origin()[i]) * invDir[i];//ѭ����x��y��z���tmin
        float max = (pMax[i] - ray.origin()[i]) * invDir[i];//tmax
        //������ߴ�������Ϊ��������pmaxΪ�����ᣬpminΪ�뿪�ᡣ����min��max����
        if (!dirIsNeg[i])
        {
            std::swap(min, max);
        }
        tEnter = std::max(min, tEnter);
        tExit = std::min(max, tExit);
    }
    return tEnter <= tExit&& tExit >= 0;//һ��=�ž������񣿣�����
}

//��Χ������Χ�е� ��Χ��
inline Bounds3 Union(const Bounds3& b1, const Bounds3& b2)
{
    Bounds3 ret;
    ret.pMin = vec3::Min(b1.pMin, b2.pMin);
    ret.pMax = vec3::Max(b1.pMax, b2.pMax);
    return ret;
}

//��Χ��Χ�к�p��� ��Χ��
inline Bounds3 Union(const Bounds3& b, const vec3& p)
{
    Bounds3 ret;
    ret.pMin = vec3::Min(b.pMin, p);
    ret.pMax = vec3::Max(b.pMax, p);
    return ret;
}

#endif // BOUNDS3_H
