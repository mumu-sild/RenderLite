#include <QMouseEvent>
#include "glwidget.h"
#include "triangle.h"
#include "rectangle.h"

void GLWidget::importModel(QString modelPath)
{
    makeCurrent();
    scene.Add(new Model(modelPath));
    scene.shaderPrograms.push_back(shaderSelector.getShader(defaultshader));
    scene.Add(new PointLight(scene.objects.last()->getlightpos(),QVector3D(1,1,1)));
    update();
    doneCurrent();
}

void GLWidget::importTriangle()
{
    makeCurrent();
    scene.Add(new Triangle());
    scene.shaderPrograms.push_back(shaderSelector.getShader(defaultshader));
    scene.Add(new PointLight(scene.objects.last()->getlightpos(),QVector3D(1,1,1)));
    update();
    doneCurrent();
}

void GLWidget::importRectangle()
{
    makeCurrent();
    scene.Add(new rectangle(10,10));
    scene.shaderPrograms.push_back(shaderSelector.getShader(defaultshader));
    scene.Add(new PointLight(scene.objects.last()->getlightpos(),QVector3D(1,1,1)));
    update();
    doneCurrent();
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    m_lastPos = event->pos();
    //鼠标左键按下事件
    if(event->button()==Qt::LeftButton)
    {
       pixX = event->pos().x();
       pixY = event->pos().y();

       makeCurrent();
       glReadPixels(pixX,height()-pixY,1,1,GL_STENCIL_INDEX,GL_INT,&mousePressObjNumber);
       doneCurrent();
    }
}

void GLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton)
    {
       if(abs(event->pos().x()-pixX)<mouseClickOffset&&abs(event->pos().y()-pixY)<mouseClickOffset)
       {
           setPixObjectNumber(event->pos().x(),event->pos().y());
       }
    }
    update();
}



void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    int dx = m_lastPos.x() - event->x();
    int dy = m_lastPos.y() - event->y();

    //摄像机旋转
    if(currentIndex==1)
    {
        if(QApplication::keyboardModifiers()==Qt::AltModifier
                &&event->buttons() == Qt::LeftButton)
        {
            maincamera.rotateCamera(dx,dy);
            update();
        }
    }

    //物体旋转
        if(currentIndex==0 && objectNumber!=0&&
                event->buttons() == Qt::LeftButton)
        {
            if(QApplication::keyboardModifiers()==Qt::AltModifier)
            {
                if(xrotation){
                    qDebug()<<"物体的x轴旋转";
                    scene.objects.at(objectNumber-1)->model.rotate(0,dx*0.5);
                }
                if(yrotation){
                    qDebug()<<"物体的y轴旋转";
                    scene.objects.at(objectNumber-1)->model.rotate(1,dx*0.5);
                }
                if(zrotation){
                    qDebug()<<"物体的z轴旋转";
                    scene.objects.at(objectNumber-1)->model.rotate(2,dx*0.5);
                }
                emit objectRotationChanged(scene.objects.at(objectNumber-1)
                                       ->model.getRotate());
                update();
            }
            else
            {
                qDebug()<<"objectNumber:"<<objectNumber;
                qDebug()<<"mousePressObjNumber"<<mousePressObjNumber;

                if(objectNumber==mousePressObjNumber)
                {
                    calCulateModelMoveCoefficient();
                    QVector3D moveObj;
                    QVector3D moveObjX = maincamera.getcameraX().normalized();
                    QVector3D moveObjY = maincamera.getcameraY().normalized();
                    moveObj = (moveObjX*dx+moveObjY*dy)*modelMoveCoefficient*0.001;
                    scene.objects.at(objectNumber-1)->model.translate(moveObj);
                    update();
                    objectChangEmitSignal();
                }
            }

        }

    m_lastPos = event->pos();
}

//鼠标滚轮滚动事件
void GLWidget::wheelEvent(QWheelEvent *event)
{
    if(currentIndex==1)
    {
        if(event->delta()>0)
        {
            qDebug()<<"滚轮往前滚";
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
            qDebug()<<"滚轮往后滚";
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

            emit objectScaleChanged(scene.objects.at(objectNumber-1)
                                   ->model.getScale());
            update();
        }
        if(event->delta()>0&&objectNumber!=0)
        {
            scene.objects.at(objectNumber-1)->model.scale(QVector3D(1.1,1.1,1.1));
            emit objectScaleChanged(scene.objects.at(objectNumber-1)
                                               ->model.getScale());
            update();
        }

    }
}

//键盘按下事件
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
            qDebug()<<"按下S键";
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
    if(event->key()==Qt::Key_Delete&&objectNumber){
        emit objectIsDeleted(objectNumber);
    }

}

void GLWidget::calCulateModelMoveCoefficient()
{
    if(objectNumber>0)
    {
        QVector3D DistenceSquare = maincamera.getCameraPos()-scene.objects.at(objectNumber-1)->model.getPosition();
        modelMoveCoefficient = DistenceSquare.length();
        qDebug()<<"modelMoveCoefficient:"<<modelMoveCoefficient;
    }

}

//键盘松开事件
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

void GLWidget::deleteObject(int objectNumber)
{
    delete scene.objects.at(objectNumber);
    scene.objects.removeAt(objectNumber);
    scene.shaderPrograms.removeAt(objectNumber);
    delete scene.pointlights.at(objectNumber);
    scene.pointlights.removeAt(objectNumber);
    update();
}
