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
    QVector3D dirZ;//����
    QVector3D dirX;//����
    QVector3D dirY;//�Ϸ�
public:
    //���캯��
    camera();

    //���ӽ�ǰ���ƶ�
    void moveForBackward(float meters);
    //���ӽ������ƶ�
    void moveLRward(float meters);
    //���ӽ������ƶ�
    void moveUpDownward(float meters);

    //�Խ���Ϊ���Ķ������λ�ý�����ת
    void rotateCamera(float xRotate, float yRotate);
    //�������Ϊ���ĶԽ��������ת

    //�����������λ��
    void setXCameraPosi(double meters);
    void setYCameraPosi(double meters);
    void setZCameraPosi(double meters);

    //�������������λ��
    void setXCameraFocus(double meters);
    void setYCameraFocus(double meters);
    void setZCameraFocus(double meters);

    //��ȡlookat����
    QMatrix4x4 getViewMetrix();
    //��ȡ�����λ��
    QVector3D getCameraPos();
    //��ȡ���������
    QVector3D getCameraFocus();

    //����xyz��
    void resetDirXYZ();
};

#endif // CAMERA_H