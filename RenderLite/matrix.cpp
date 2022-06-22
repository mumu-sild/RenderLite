#include "matrix.h"


void mat4::setZero()
{
    for(int i=0;i<4;i++){
        for(int j=0;j<4;j++){
            e[i][j] = 0;
        }
    }
}

void mat4::setIdentity()
{
    for(int i=0;i<4;i++){
        for(int j=0;j<4;j++){
            if(i==j)e[i][j]=1;
            else e[i][j] = 0;
        }
    }
}

mat4 mat4::transpose(mat4 &m)
{
    return mat4(1.0);
}
