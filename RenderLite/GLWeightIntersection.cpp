#include <QMouseEvent>
#include "glwidget.h"

void GLWidget::importModel(QString modelPath)
{
    makeCurrent();
    scene.Add(new Model(modelPath));
    scene.shaderProgram.push_back(shaderSelector.getShader());
    update();
    doneCurrent();
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    m_lastPos = event->pos();
    //�����������¼�
    if(event->button()==Qt::LeftButton)
    {
       pixX = event->pos().x();
       pixY = event->pos().y();
    }
}

void GLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton)
    {
       if(abs(event->pos().x()-pixX)<1&&abs(event->pos().y()-pixY)<1)
       {
           setPixObjectNumber(event->pos().x(),event->pos().y());
       }
    }
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    int dx = m_lastPos.x() - event->x();
    int dy = m_lastPos.y() - event->y();

    //�������ת
    if(currentIndex==1)
    {
        if(QApplication::keyboardModifiers()==Qt::AltModifier
                &&event->buttons() == Qt::LeftButton)
        {
            maincamera.rotateCamera(dx,dy);
            update();
        }
    }

    //������ת
    if(currentIndex==0 && objectNumber!=0)
    {
        if(xrotation&&QApplication::keyboardModifiers()==Qt::AltModifier
                &&event->buttons() == Qt::LeftButton)
        {
            qDebug()<<"�����x����ת";
            scene.objects.at(objectNumber-1)->model.rotate(0,dx*0.5);
            update();
        }
        if(yrotation&&QApplication::keyboardModifiers()==Qt::AltModifier
                &&event->buttons() == Qt::LeftButton)
        {
            qDebug()<<"�����y����ת";
            scene.objects.at(objectNumber-1)->model.rotate(1,dx*0.5);
            update();
        }
        if(zrotation&&QApplication::keyboardModifiers()==Qt::AltModifier
                &&event->buttons() == Qt::LeftButton)
        {
            qDebug()<<"�����z����ת";
            scene.objects.at(objectNumber-1)->model.rotate(2,dx*0.5);
            update();
        }
    }

    m_lastPos = event->pos();
}

//�����ֹ����¼�
void GLWidget::wheelEvent(QWheelEvent *event)
{
    if(currentIndex==1)
    {
        if(event->delta()>0)
        {
            qDebug()<<"������ǰ��";
            maincamera.moveForBackward(-1);

            emit xCameraPosiChanged(maincamera.getCameraPos().x());
            emit yCameraPosiChanged(maincamera.getCameraPos().y());
            emit zCameraPosiChanged(maincamera.getCameraPos().z());

            emit xCameraFocusChanged(maincamera.getCameraFocus().x());
            emit yCameraFocusChanged(maincamera.getCameraFocus().y());
            emit zCameraFocusChanged(maincamera.getCameraFocus().z());

            qDebug()<<maincamera.getCameraPos().x();
            qDebug()<<maincamera.getCameraPos().y();
            qDebug()<<maincamera.getCameraPos().z();

            update();
        }
        if(event->delta()<0)
        {
            qDebug()<<"���������";
            maincamera.moveForBackward(1);

            emit xCameraPosiChanged(maincamera.getCameraPos().x());
            emit yCameraPosiChanged(maincamera.getCameraPos().y());
            emit zCameraPosiChanged(maincamera.getCameraPos().z());

            emit xCameraFocusChanged(maincamera.getCameraFocus().x());
            emit yCameraFocusChanged(maincamera.getCameraFocus().y());
            emit zCameraFocusChanged(maincamera.getCameraFocus().z());

            qDebug()<<maincamera.getCameraPos().x();
            qDebug()<<maincamera.getCameraPos().y();
            qDebug()<<maincamera.getCameraPos().z();

            update();
        }
    }
    if(currentIndex==0)
    {
        if(event->delta()<0&&objectNumber!=0)
        {
            scene.objects.at(objectNumber-1)->model.scale(QVector3D(0.9,0.9,0.9));



            update();
        }
        if(event->delta()>0&&objectNumber!=0)
        {
            scene.objects.at(objectNumber-1)->model.scale(QVector3D(1.1,1.1,1.1));
            update();
        }

    }
}

//���̰����¼�
void GLWidget::keyPressEvent(QKeyEvent *event)
{
    if(currentIndex==1)
    {
        if(event->key()==Qt::Key_W)
        {
            maincamera.moveUpDownward(1);

            emit xCameraPosiChanged(maincamera.getCameraPos().x());
            emit yCameraPosiChanged(maincamera.getCameraPos().y());
            emit zCameraPosiChanged(maincamera.getCameraPos().z());

            emit xCameraFocusChanged(maincamera.getCameraFocus().x());
            emit yCameraFocusChanged(maincamera.getCameraFocus().y());
            emit zCameraFocusChanged(maincamera.getCameraFocus().z());

            qDebug()<<maincamera.getCameraPos().x();
            qDebug()<<maincamera.getCameraPos().y();
            qDebug()<<maincamera.getCameraPos().z();

            update();
        }
        if(event->key()==Qt::Key_S)
        {
            qDebug()<<"����S��";
            maincamera.moveUpDownward(-1);

            emit xCameraPosiChanged(maincamera.getCameraPos().x());
            emit yCameraPosiChanged(maincamera.getCameraPos().y());
            emit zCameraPosiChanged(maincamera.getCameraPos().z());

            emit xCameraFocusChanged(maincamera.getCameraFocus().x());
            emit yCameraFocusChanged(maincamera.getCameraFocus().y());
            emit zCameraFocusChanged(maincamera.getCameraFocus().z());

            qDebug()<<maincamera.getCameraPos().x();
            qDebug()<<maincamera.getCameraPos().y();
            qDebug()<<maincamera.getCameraPos().z();

            update();
        }
        if(event->key()==Qt::Key_A)
        {
            maincamera.moveLRward(-1);

            emit xCameraPosiChanged(maincamera.getCameraPos().x());
            emit yCameraPosiChanged(maincamera.getCameraPos().y());
            emit zCameraPosiChanged(maincamera.getCameraPos().z());

            emit xCameraFocusChanged(maincamera.getCameraFocus().x());
            emit yCameraFocusChanged(maincamera.getCameraFocus().y());
            emit zCameraFocusChanged(maincamera.getCameraFocus().z());

            qDebug()<<maincamera.getCameraPos().x();
            qDebug()<<maincamera.getCameraPos().y();
            qDebug()<<maincamera.getCameraPos().z();

            update();
        }
        if(event->key()==Qt::Key_D)
        {
            maincamera.moveLRward(1);

            emit xCameraPosiChanged(maincamera.getCameraPos().x());
            emit yCameraPosiChanged(maincamera.getCameraPos().y());
            emit zCameraPosiChanged(maincamera.getCameraPos().z());

            emit xCameraFocusChanged(maincamera.getCameraFocus().x());
            emit yCameraFocusChanged(maincamera.getCameraFocus().y());
            emit zCameraFocusChanged(maincamera.getCameraFocus().z());

            qDebug()<<maincamera.getCameraPos().x();
            qDebug()<<maincamera.getCameraPos().y();
            qDebug()<<maincamera.getCameraPos().z();

            update();
        }
    }

    if(currentIndex==0)
    {
        if(event->key()==Qt::Key_W&&objectNumber!=0)
        {
            QVector3D y = maincamera.getcameraY().normalized();
            scene.objects.at(objectNumber-1)->model.translate(y);

            emit objectPosiChanged(scene.objects.at(objectNumber-1)
                                   ->model.getPosition());

            update();
        }
        if(event->key()==Qt::Key_S&&objectNumber!=0)
        {
            QVector3D y = maincamera.getcameraY().normalized();
            scene.objects.at(objectNumber-1)->model.translate(-y);

            emit objectPosiChanged(scene.objects.at(objectNumber-1)
                                   ->model.getPosition());

            update();
        }
        if(event->key()==Qt::Key_A&&objectNumber!=0)
        {
            QVector3D x = maincamera.getcameraX().normalized();
            scene.objects.at(objectNumber-1)->model.translate(x);

            emit objectPosiChanged(scene.objects.at(objectNumber-1)
                                   ->model.getPosition());

            update();
        }
        if(event->key()==Qt::Key_D&&objectNumber!=0)
        {
            QVector3D x = maincamera.getcameraX().normalized();
            scene.objects.at(objectNumber-1)->model.translate(-x);

            emit objectPosiChanged(scene.objects.at(objectNumber-1)
                                   ->model.getPosition());

            update();
        }
    }


}

//�����ɿ��¼�
void GLWidget::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key()==Qt::Key_W)
    {
    }
    if(event->key()==Qt::Key_S)
    {
    }
    if(event->key()==Qt::Key_A)
    {
    }
    if(event->key()==Qt::Key_D)
    {
    }
}
int GLWidget::getObjectSize()
{
    return scene.objects.size();
}

void GLWidget::setXCameraPosi(double meters)
{
    maincamera.setXCameraPosi(meters);
    update();
}

void GLWidget::setYCameraPosi(double meters)
{
    maincamera.setYCameraPosi(meters);
    update();
}

void GLWidget::setZCameraPosi(double meters)
{
    maincamera.setZCameraPosi(meters);
    update();
}

void GLWidget::setXCameraFocus(double meters)
{
    maincamera.setXCameraFocus(meters);
    update();
}

void GLWidget::setYCameraFocus(double meters)
{
    maincamera.setYCameraFocus(meters);
    update();
}

void GLWidget::setZCameraFocus(double meters)
{
    maincamera.setZCameraFocus(meters);
    update();
}
