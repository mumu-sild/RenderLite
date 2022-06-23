#ifndef VEC_H
#define VEC_H

#include <iostream>
#include <math.h>

using namespace std;

/**************************************************************************
**
**                                  vec3
**
***************************************************************************/
class vec4;
class vec3;
class vec2;

class vec3 {
public:
    //参数
    double e[3];

public:
    //构造函数
    vec3() :e{ 0, 0, 0} {};//列表初始化
    vec3(double x, double y, double z) {
        e[0] = x;
        e[1] = y;
        e[2] = z;
    };
    //返回成员数值函数
    double x() const { return e[0]; }
    double y() const { return e[1]; }
    double z() const { return e[2]; }
    //返回成员函数
    double& operator[](int i) { return e[i]; }
    double operator[](int i) const {return e[i];}
    //运算函数+ - * /
    vec3 operator-() const;//末尾const表示不改变内部成员
    vec3& operator+=(const vec3&);
    vec3& operator-=(const vec3&);
    vec3& operator*=(const double&);
    vec3& operator/=(const double&);
    //输出函数
    friend ostream& operator<<(ostream& out, vec3& v);//只能使用友元函数
    //性质函数
    double length() const;
    double length_squared() const;
    vec3 normalized();

    bool near_zero() const {
        // Return true if the vector is close to zero in all dimensions.
        const auto s = 1e-8;
        return (fabs(e[0]) < s) && (fabs(e[1]) < s) && (fabs(e[2]) < s);
    }

    //类内静态函数
    static vec3 Min(const vec3 &p1, const vec3 &p2) {
            return vec3(std::min(p1.x(), p2.x()), std::min(p1.y(), p2.y()),
                           std::min(p1.z(), p2.z()));
        }

    static vec3 Max(const vec3 &p1, const vec3 &p2) {
        return vec3(std::max(p1.x(), p2.x()), std::max(p1.y(), p2.y()),
                       std::max(p1.z(), p2.z()));
    }
};

// Type aliases for vec3
using point3 = vec3;   // 3D point
using color = vec3;    // RGB color

inline double dot(const vec3& u, const vec3& v) {
    return v.e[0] * u.e[0] + v.e[1] * u.e[1] + v.e[2] * u.e[2];
}

//u X v = u->v的拇指方向
inline vec3 cross(const vec3& u, const vec3& v) {
    return  vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
        u.e[2] * v.e[0] - u.e[0] * v.e[2],
        u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}

inline vec3 operator+(const vec3& u, const vec3& v) {
    return vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}

inline vec3 operator-(const vec3& u, const vec3& v) {
    return vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

inline vec3 operator*(const vec3& u, const vec3& v) {
    return vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}

inline vec3 operator*(double t, const vec3& v) {
    return vec3(t * v.e[0], t * v.e[1], t * v.e[2]);
}

inline vec3 operator*(const vec3& v, double t) {
    return t * v;
}

inline vec3 operator/(vec3 v, double t) {
    return (1 / t) * v;
}

inline ostream& operator<<(ostream& out, vec3& v)
{
    out << "[" << v.e[0] << " " << v.e[1] << " " << v.e[2] << "]" << endl;
    return out;
}



inline vec3 normalize(vec3 v) {
    return v / v.length();
}

/****************************************************************************
**
**                                  vec2
**
*****************************************************************************/
class vec2 {
public:
    //参数
    double e[2];

public:
    //构造函数
    vec2() :e{0, 0} {};//列表初始化
    vec2(double x, double y) {
        e[0] = x;
        e[1] = y;
    };
    //返回成员数值函数
    double x() { return e[0]; }
    double y() { return e[1]; }
    //返回成员引用函数
    double& operator[](int i) { return e[i]; }
    //运算函数+ - * /
    vec2 operator-() const;//末尾const表示不改变内部成员
    vec2& operator+=(const vec2&);
    vec2& operator-=(const vec2&);
    vec2& operator*=(const double&);
    vec2& operator/=(const double&);
    //输出函数
    friend ostream& operator<<(ostream& out, vec2& v);//只能使用友元函数
    //性质函数
    float length() const;
    float length_squared() const;
    vec2 normalized();

    bool near_zero() const {
        // Return true if the vector is close to zero in all dimensions.
        const auto s = 1e-8;
        return (fabs(e[0]) < s) && (fabs(e[1]) < s);
    }
};
/****************************************************************************
**
**                                  vec4
**
*****************************************************************************/
class vec4 {
private:
    //参数
    double e[4];

public:
    //构造函数
    vec4() :e{0, 0, 0, 0} {};//列表初始化
    vec4(double x, double y,double z,double w) {
        e[0] = x;
        e[1] = y;
        e[2] = z;
        e[3] = w;
    };
    vec4(vec3 &v, double w){
        e[0] = v[0];
        e[1] = v[1];
        e[2] = v[2];
        e[3] = w;
    }
    //返回成员数值函数
    double x() { return e[0]; }
    double y() { return e[1]; }
    double z() { return e[2]; }
    double w() { return e[3]; }
    //返回成员引用函数
    double& operator[](int i) { return e[i]; }
    //运算函数+ - * /
    vec4 operator-() const;//末尾const表示不改变内部成员
    vec4& operator+=(const vec4&);
    vec4& operator-=(const vec4&);
    vec4& operator*=(const double&);
    vec4& operator/=(const double&);
    //输出函数
    friend ostream& operator<<(ostream& out, vec4& v);//只能使用友元函数

};

using quaternions = vec4;    // 四元数


#endif // VEC_H
