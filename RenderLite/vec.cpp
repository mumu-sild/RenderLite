#include "vec.h"

/****************************************************************************
**
**                                  vec3
**
*****************************************************************************/

inline double vec3::length() const {
    return sqrt(length_squared());
}

inline double vec3::length_squared() const {
    return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
}

inline vec3 vec3::operator-() const
{
    return vec3(-e[0], -e[1], -e[2]);
}

inline vec3& vec3::operator+=(const vec3& a)
{
    e[0] += a.e[0];
    e[1] += a.e[1];
    e[2] += a.e[2];
    return *this;
}

inline vec3& vec3::operator-=(const vec3& a)
{
    e[0] -= a.e[0];
    e[1] -= a.e[1];
    e[2] -= a.e[2];
    return *this;
}

inline vec3& vec3::operator*=(const double& t) {
    e[0] *= t;
    e[1] *= t;
    e[2] *= t;
    return *this;
}

inline vec3& vec3::operator/=(const double& t) {
    return *this *= 1 / t;
}

inline vec3 vec3::normalized()
{
    *this /= this->length();
    return *this;
}

/****************************************************************************
**
**                                  vec2
**
*****************************************************************************/

inline float vec2::length() const {
    return sqrt(length_squared());
}

inline float vec2::length_squared() const {
    return e[0] * e[0] + e[1] * e[1];
}

inline vec2 vec2::operator-() const
{
    return vec2(-e[0], -e[1]);
}

inline vec2& vec2::operator+=(const vec2& a)
{
    e[0] += a.e[0];
    e[1] += a.e[1];
    return *this;
}

inline vec2& vec2::operator-=(const vec2& a)
{
    e[0] -= a.e[0];
    e[1] -= a.e[1];
    return *this;
}

inline vec2& vec2::operator*=(const double& t) {
    e[0] *= t;
    e[1] *= t;
    return *this;
}

inline vec2& vec2::operator/=(const double& t) {
    return *this *= 1 / t;
}

inline vec2 vec2::normalized()
{
    *this /= this->length();
    return *this;
}


/****************************************************************************
**
**                                  vec4
**
*****************************************************************************/

inline vec4 vec4::operator-() const
{
    return vec4(-e[0], -e[1],-e[2], e[3]);
}

inline vec4& vec4::operator+=(const vec4& a)
{
    e[0] += a.e[0];
    e[1] += a.e[1];
    e[2] += a.e[2];
    e[3] += a.e[3];
    return *this;
}

inline vec4& vec4::operator-=(const vec4& a)
{
    e[0] -= a.e[0];
    e[1] -= a.e[1];
    e[2] -= a.e[2];
    e[3] -= a.e[3];
    return *this;
}

inline vec4& vec4::operator*=(const double& t) {
    e[0] *= t;
    e[1] *= t;
    e[2] *= t;
    e[3] *= t;
    return *this;
}

inline vec4& vec4::operator/=(const double& t) {
    return *this *= 1 / t;
}

