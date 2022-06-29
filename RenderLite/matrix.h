#ifndef MATRIX_H
#define MATRIX_H

//rotate,lookAt->camera
#include <iostream>
#include "vec.h"

using namespace std;

class mat4
{
public:
    vec4 m[4];//为四个列向量

public:
    mat4():m{{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}}{};
    mat4(double n){
        for(int i=0;i<4;i++){
            for(int j=0;j<4;j++){
                m[i][j] = n;
            }
        }
    }
    void setZero();
    void setIdentity();
    //返回成员引用函数
    double& operator()(int i,int j) { return m[i][j]; }
    vec4& operator[](int i){return m[i];}
    vec4 operator[] (int i) const {return m[i];}
    friend ostream& operator<<(ostream& out, vec3& v);//只能使用友元函数
    mat4 transpose(mat4 &m);

};

inline ostream& operator<<(ostream& out, mat4& m)
{
    for(int i=0;i<4;i++){
        out << "[" << m(i,0) << " " << m(i,1) << " " << m(i,2) \
            << " " << m(i,3) << "]" << endl;
    }

    return out;
}

//mat4 rotate(const mat4& m,float angle,const vec3& v){
//    const float a = angle;
//    const float cosa = cos(a);
//    const float sina = sin(a);

//    vec3 axis(normalize((v)));
//    vec3 temp((1.0f-cosa)*axis);

//    mat4 Rotate;
//    Rotate(0,0) = cosa + temp[0] * axis[0];
//    Rotate(0,1) = temp[0] * axis[1] + sina * axis[2];
//    Rotate(0,2) = temp[0] * axis[2] - sina * axis[1];

//    Rotate(1,0) = temp[1] * axis[0] - sina * axis[2];
//    Rotate(1,1) = cosa + temp[1] * axis[1];
//    Rotate(1,2) = temp[1] * axis[2] + sina * axis[0];

//    Rotate(2,0) = temp[2] * axis[0] + sina * axis[1];
//    Rotate(2,1) = temp[2] * axis[1] - sina * axis[0];
//    Rotate(2,2) = cosa + temp[2] * axis[2];

//    mat4 Result;
//    Result[0] = m[0] * Rotate[0][0] + m[1] * Rotate[0][1] + m[2] * Rotate[0][2];
//    Result[1] = m[0] * Rotate[1][0] + m[1] * Rotate[1][1] + m[2] * Rotate[1][2];
//    Result[2] = m[0] * Rotate[2][0] + m[1] * Rotate[2][1] + m[2] * Rotate[2][2];
//    Result[3] = m[3];

//    return Result;
//}

/*矩阵与向量的乘法*/
vec4 operator*(const mat4 &m,const vec4 &v){
    vec4 result;
    for(int i=0;i<4;i++){
        for(int j=0;j<4;j++){
            result[i] +=m[j][i]*v[j];
        }
    }
    return result;
}



//rotate,transform,scale

#endif // MATRIX_H
