
#include <QVector3D>
#include <QVector4D>
#include <QMatrix4x4>
#include "camera.h"
#include "Global.h"
#include "Setting.h"

camera::camera() :cameraPosi(camereInitialPosition), cameraFocus(cameraInitialFocus),
                    cameraUp(cameraInitUp)
{
    dirZ = (cameraPosi - cameraFocus).normalized();
    dirX = (QVector3D::crossProduct(dirZ, cameraUp));
    dirY = -(QVector3D::crossProduct(dirZ, dirX)).normalized();
    //projection.perspective(45.0f, 800 / 1200, 0.001f, 1000.0f);
}

void camera::moveForBackward(float meters)
{
    cameraPosi += meters * dirZ;
    cameraFocus += meters * dirZ;
}

void camera::moveLRward(float meters)
{
    cameraPosi  += meters * dirX;//meters为正时，向左移动。
    cameraFocus += meters * dirX;
}

void camera::moveUpDownward(float meters)
{
    cameraPosi += meters * dirY;
    cameraFocus += meters * dirY;
}


//xRotate为屏幕x偏移量，yRotate为屏幕y偏移量
void camera::rotateCamera(float xRotate, float yRotate)
{
    QVector4D cameraPosi4v = QVector4D(cameraPosi, 1.0f);
    QVector4D cameraFocus4v = QVector4D(cameraFocus, 1.0f);
    QVector4D dirY4v = QVector4D(dirY, 1.0f);


    //可优化
        //dir = dirY*xRotate+dirX*yRotate;
        //Rotate = xRotate*xRotate +yRotate*yRotate;
        //rotateMetrix = rotate(rotateMetrix,degree_to_radians(Rotate),-dir)
    //
    QMatrix4x4 rotateMetrix;
    rotateMetrix.setToIdentity();
    rotateMetrix.rotate(xRotate, cameraUp);
    rotateMetrix.rotate(yRotate, -dirX);

    cameraPosi4v = rotateMetrix * cameraPosi4v;
    cameraFocus4v = rotateMetrix * cameraFocus4v;
    dirY4v = rotateMetrix * dirY4v;

    dirY = QVector3D(dirY4v.x(), dirY4v.y(), dirY4v.z());
    cameraPosi =QVector3D(cameraPosi4v.x(), cameraPosi4v.y(), cameraPosi4v.z());
    cameraFocus = QVector3D(cameraFocus4v.x(),cameraFocus4v.y(),cameraFocus4v.z());
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


QVector3D camera::getCameraFocus()
{
    return cameraFocus;
}


void camera::setXCameraPosi(double meters)
{
    cameraPosi.setX(meters);
}

void camera::setYCameraPosi(double meters)
{
    cameraPosi.setY(meters);
}

void camera::setZCameraPosi(double meters)
{
    cameraPosi.setZ(meters);
}

void camera::setXCameraFocus(double meters)
{
    cameraFocus.setX(meters);
}

void camera::setYCameraFocus(double meters)
{
    cameraFocus.setY(meters);
}

void camera::setZCameraFocus(double meters)
{
    cameraFocus.setZ(meters);
}

QVector3D camera::getcameraX()
{
    return dirX;
}

QVector3D camera::getcameraY(){
    return dirY;
}


