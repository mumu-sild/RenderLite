#ifndef MATRIX_H
#define MATRIX_H

//rotate,lookAt->camera
#include <iostream>
#include "vec.h"

using namespace std;

class mat4
{
private:
    double e[4][4];

public:
    mat4():e{{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}}{};
    mat4(double i){
        for(int i=0;i<4;i++){
            for(int j=0;j<4;j++){
                e[i][j] = i;
            }
        }
    }
    void setZero();
    void setIdentity();
    //���س�Ա���ú���
    double& operator()(int i,int j) { return e[i][j]; }
    friend ostream& operator<<(ostream& out, vec3& v);//ֻ��ʹ����Ԫ����
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

#endif // MATRIX_H
