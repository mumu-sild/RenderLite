#ifndef CAMERA_H
#define CAMERA_H

#include <QVector3D>
#include <QMatrix4x4>

class camera
{
private:
    QVector3D cameraPosi;
    QVector3D cameraFocus;
    QVector3D cameraUp;
    QVector3D dirZ;//正向
    QVector3D dirX;//右向
    QVector3D dirY;//上方
public:
    //构造函数
    camera();

    //向视角前后移动
    void moveForBackward(float meters);
    //向视角左右移动
    void moveLRward(float meters);
    //向视角上下移动
    void moveUpDownward(float meters);

    //以交点为中心对摄像机位置进行旋转
    void rotateCamera(float xRotate, float yRotate);
    //以摄像机为中心对焦点进行旋转

    //获取lookat矩阵
    QMatrix4x4 getViewMetrix();
    //获取摄像机位置
    QVector3D getCameraPos();

    //重设xyz轴
    void resetDirXYZ();
};

#endif // CAMERA_H
