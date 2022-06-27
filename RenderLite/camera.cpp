
#include <QVector3D>
#include <QVector4D>
#include <QMatrix4x4>
#include "camera.h"
#include "Global.h"

camera::camera() :cameraPosi{ 0.0f,0.0f,0.3f }, cameraFocus{ 0.0f,0.0f,0.0f },
                    cameraUp{ 0.0f,0.1f,0.0f }
{
    dirZ = (cameraPosi - cameraFocus).normalized();
    dirX = (QVector3D::crossProduct(dirZ, cameraUp));
    dirY = -(QVector3D::crossProduct(dirZ, dirX)).normalized();
}

void camera::moveForBackward(float meters)
{
    if ((cameraPosi-cameraFocus).length() >= 0.3f) {
        cameraPosi += meters * dirZ;
        //printf("distanceΪ%f\n", glm::distance(cameraPosi, cameraFocus));
    }
    else {
        cameraPosi = dirZ * 0.3f + cameraFocus;
    }
}

void camera::moveLRward(float meters)
{
    cameraPosi  += meters * dirX;//metersΪ��ʱ�������ƶ���
    cameraFocus += meters * dirX;
}

void camera::moveUpDownward(float meters)
{
    cameraPosi += meters * dirY;
    cameraFocus += meters * dirY;
}


//xRotateΪ��Ļxƫ������yRotateΪ��Ļyƫ����
void camera::rotateCamera(float xRotate, float yRotate)
{
    QVector4D cameraPosi4v = QVector4D(cameraPosi, 1.0f);
    QVector4D dirY4v = QVector4D(dirY, 1.0f);


    //���Ż�
        //dir = dirY*xRotate+dirX*yRotate;
        //Rotate = xRotate*xRotate +yRotate*yRotate;
        //rotateMetrix = rotate(rotateMetrix,degree_to_radians(Rotate),-dir)
    //
    QMatrix4x4 rotateMetrix;
    rotateMetrix.setToIdentity();
    rotateMetrix.rotate(xRotate, -dirY);
    cameraPosi4v = rotateMetrix * cameraPosi4v;

    rotateMetrix.setToIdentity();
    rotateMetrix.rotate(yRotate, -dirX);
    cameraPosi4v = rotateMetrix * cameraPosi4v;
    dirY4v = rotateMetrix * dirY4v;

    dirY = QVector3D(dirY4v.x(), dirY4v.y(), dirY4v.z());
    cameraPosi = QVector3D(cameraPosi4v.x(), cameraPosi4v.y(), cameraPosi4v.z());
    resetDirXYZ();

}

QMatrix4x4 camera::getViewMetrix()
{
    QMatrix4x4 lookatMatrix;
    lookatMatrix.lookAt(cameraPosi,cameraFocus,dirY);
    return lookatMatrix;
}

void camera::resetDirXYZ()
{
    dirY.normalize();
    dirZ = (cameraPosi - cameraFocus).normalized();
    dirX = (QVector3D::crossProduct(dirZ, dirY)).normalized();
}

QVector3D camera::getCameraPos()
{
    return cameraPosi;
}

