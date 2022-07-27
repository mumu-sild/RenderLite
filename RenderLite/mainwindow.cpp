#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDesktopWidget>
#include <QApplication>
#include <QAction>
#include <QHBoxLayout>
#include <QWidget>
#include <QTabWidget>
#include <QDebug>
#include <QFileDialog>
#include <QDesktopServices>
#include <QLineEdit>
#include <QMessageBox>



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    glWidget = new GLWidget;
    setUiStyle();
    this->showMaximized();

    ui->tabWidget->setCurrentIndex(1);

    ui->object_renderMethod_comboBox->addItem("零号渲染");
    ui->object_renderMethod_comboBox->addItem("一号渲染");
    ui->object_renderMethod_comboBox->addItem("二号渲染");
    ui->object_renderMethod_comboBox->addItem("三号渲染");
    ui->object_color_widget->setHidden(true);

    ui->object_number_comboBox->addItem("无");
    for(;objectNumSize<=glWidget->getObjectSize();objectNumSize++)
    {
        ui->object_number_comboBox->addItem(tr("%1").arg(objectNumSize));
    }

    ui->light_direction_x->setValue(glWidget->scene.dirlight->getDirection().x());
    ui->light_direction_y->setValue(glWidget->scene.dirlight->getDirection().y());
    ui->light_direction_z->setValue(glWidget->scene.dirlight->getDirection().z());
    ui->dirlight_color_red->setValue(glWidget->scene.dirlight->getColor().x()*255);
    ui->dirlight_color_green->setValue(glWidget->scene.dirlight->getColor().y()*255);
    ui->dirlight_color_blue->setValue(glWidget->scene.dirlight->getColor().z()*255);



    //对话框
    QWidget *editObjectNameWidget = new QWidget;
    editObjectNameWidget->resize(600,400);
    QLabel *objNameInputLabel = new QLabel(editObjectNameWidget);
    objNameInputLabel->resize(200,30);
    objNameInputLabel->move(200,160);
    objNameInputLabel->setText("输入当前模型名称");
    QLineEdit *objNameInput = new QLineEdit(editObjectNameWidget);
    objNameInput->resize(200,30);
    objNameInput->move(200,200);
    QPushButton *sureBtn = new QPushButton(editObjectNameWidget);
    sureBtn->setText("确定");
    sureBtn->resize(100,30);
    sureBtn->move(230,300);

    connect(ui->edit_object_name,&QAction::triggered,[=](){
        if(glWidget->objectNumber>0)
        {
           editObjectNameWidget->show();
           objNameInput->setText(ui->object_number_comboBox->currentText());
           sureBtn->setFocus();
        }
    });

    connect(sureBtn,&QPushButton::clicked,[=](){
       QString name =  objNameInput->text();
       ui->object_number_comboBox->setItemText(glWidget->objectNumber,name);
       editObjectNameWidget->close();
    });




    //点击“导入”的动作打开文件
    connect(ui->import_file,&QAction::triggered,[=](){
        QFileDialog selectFile(this);
        if(selectFile.exec()==0)
            return;
        QStringList filePaths = selectFile.selectedFiles();
        //theFilePath为所要的路径
        QString theFilePath = filePaths[0];
        glWidget->importModel(theFilePath);
        qDebug()<<"打开的模型路径"<<theFilePath;

        emit objectSizeChanged(glWidget->getObjectSize());
    });

    //导出文件为jpg格式
    connect(ui->outport_file,&QAction::triggered,[=](){
        QFileDialog outportFile(this);
        outportFile.setLabelText(QFileDialog::Accept,"保存");
        if(outportFile.exec()==0)
            return;

        QStringList outFilePaths = outportFile.selectedFiles();
        //theFilePath为所要的路径
        QString outFilePath = outFilePaths[0];
        qDebug()<<outFilePath;
        QPixmap pixmap = QPixmap::grabWidget(glWidget);
        if(pixmap.save(outFilePath,"JPG"))
        {
            QMessageBox::warning(glWidget, tr("提示"), tr("保存成功"), QMessageBox::Yes);
        }
        else{
            QMessageBox::warning(glWidget, tr("提示"), tr("保存失败"), QMessageBox::Yes);
        }
    });

    //根据摄像机的位置改变设置输入框
    connect(glWidget,&GLWidget::xCameraPosiChanged,[=](double meters){
        ui->camera_position_x_spinbox->setValue(meters);
    });
    connect(glWidget,&GLWidget::yCameraPosiChanged,[=](double meters){
        ui->camera_position_y_spinbox->setValue(meters);
    });
    connect(glWidget,&GLWidget::zCameraPosiChanged,[=](double meters){
        ui->camera_position_z_spinbox->setValue(meters);
    });
    connect(glWidget,&GLWidget::xCameraFocusChanged,[=](double meters){
        ui->camera_focus_x_spinbox->setValue(meters);
    });
    connect(glWidget,&GLWidget::yCameraFocusChanged,[=](double meters){
        ui->camera_focus_y_spinbox->setValue(meters);
    });
    connect(glWidget,&GLWidget::zCameraFocusChanged,[=](double meters){
        ui->camera_focus_z_spinbox->setValue(meters);
    });

    //根据鼠标点击的物体改变tabwidget
    connect(glWidget,&GLWidget::objectNumberChanged,[=](int objectNumber){
       if(objectNumber==0)
       {
           ui->tabWidget->setCurrentIndex(1);
       }
       else{
           ui->tabWidget->setCurrentIndex(0);
           ui->object_number_comboBox->setCurrentIndex(objectNumber);

           //设置灯光状态
           if(glWidget->scene.objects.at(objectNumber-1)->islight==false)
           {

               ui->object_is_light_chekBox->setCheckState(Qt::Unchecked);
           }
           else
           {
               ui->object_is_light_chekBox->setCheckState(Qt::Checked);
           }

           //设置使用的渲染器编号
           QOpenGLShaderProgram* shader = glWidget->scene.shaderPrograms.at(objectNumber-1);

           for(int i=0;i<glWidget->shaderSelector.vertexPath.size();++i){
               if(shader == glWidget->shaderSelector.getShader(i)){
                   ui->object_renderMethod_comboBox->setCurrentIndex(i);
                   break;
               }
           }

           //设置二号渲染器的颜色
          if(shader == glWidget->shaderSelector.getShader(2))
          {
              ui->object_color_red_spinbox->setValue(glWidget->scene.objects.at(glWidget->objectNumber-1)->color[0]*255);
              ui->object_color_green_spinbox->setValue(glWidget->scene.objects.at(glWidget->objectNumber-1)->color[1]*255);
              ui->object_color_blue_spinbox->setValue(glWidget->scene.objects.at(glWidget->objectNumber-1)->color[2]*255);
          }
       }
    });

    //根据导入模型个数新增下拉框项数
    connect(this,&MainWindow::objectSizeChanged,[=](int theObjectSize){
        for(;objectNumSize<=glWidget->getObjectSize();objectNumSize++,objectSerialNumber++)
        {
            ui->object_number_comboBox->addItem(tr("%1").arg(objectSerialNumber));
        }
    });
    //删除物体重新设置下拉框
    connect(glWidget,&GLWidget::objectIsDeleted,[=](int objectNumber){
        glWidget->deleteObject(objectNumber-1);
        ui->object_number_comboBox->removeItem(objectNumber);
        objectNumSize -= 1;
    });


    //物体位置改变设置ui
    connect(glWidget,&GLWidget::objectPosiChanged,[=](QVector3D posi){
        ui->object_position_x_spinbox->setValue(posi.x());
        ui->object_position_y_spinbox->setValue(posi.y());
        ui->object_position_z_spinbox->setValue(posi.z());
    });

    //物体旋转改变设置ui
    connect(glWidget,&GLWidget::objectRotationChanged,[=](QVector3D rotation){
        while(rotation.x()<-180)rotation[0]+=360;
        while(rotation.y()<-180)rotation[1]+=360;
        while(rotation.z()<-180)rotation[2]+=360;
        ui->object_rotation_x_spinbox->setValue(int(rotation.x()+180)%360-180);
        ui->object_rotation_y_spinbox->setValue(int(rotation.y()+180)%360-180);
        ui->object_rotation_z_spinbox->setValue(int(rotation.z()+180)%360-180);
    });

    //物体伸缩改变设置ui
    connect(glWidget,&GLWidget::objectScaleChanged,[=](QVector3D scale){
        ui->object_scale_x_spinbox->setValue(scale.x());
        ui->object_scale_y_spinbox->setValue(scale.y());
        ui->object_scale_z_spinbox->setValue(scale.z());
    });

    //新建三角形
    connect(ui->new_model_triangle,&QAction::triggered,[=](){
        glWidget->importTriangle();
        emit objectSizeChanged(glWidget->getObjectSize());
    });

    //新建矩形
    connect(ui->new_model_rectangle,&QAction::triggered,[=](){
        glWidget->importRectangle();
        emit objectSizeChanged(glWidget->getObjectSize());
    });

    //项目仓库
    connect(ui->use_specification,&QAction::triggered,[=](){
        const QUrl regUrl(QLatin1String("https://github.com/mumu-sild/RenderLite"));
        QDesktopServices::openUrl(regUrl);
    });


}

void MainWindow::setUiStyle()
{
    //设置各个窗口部件大小和位置
    //设置整个窗口和屏幕一样大,并移动位置
    this->resize(QApplication::desktop()->width()*0.7,
                 QApplication::desktop()->height()*0.8);
    this->move(QApplication::desktop()->width()*0.15,
               QApplication::desktop()->height()*0.08);
    //设置整个界面的tabwidget和glwidget
    mainWidget = new QWidget;
    setCentralWidget(mainWidget);

    ui->tabWidget->setMaximumSize(this->width()*0.2,this->height());
    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(glWidget);
    mainLayout->addWidget(ui->tabWidget);

    mainWidget->setLayout(mainLayout);


    /*设置tabwidget中的物体widget*/
    //设置布局使得物体widget随窗口大小改变
    QVBoxLayout *objectLayout = new QVBoxLayout;
    objectLayout->addWidget(ui->scrollArea1);
    ui->object->setLayout(objectLayout);
    QVBoxLayout *cameraLayout = new QVBoxLayout;
    cameraLayout->addWidget(ui->scrollArea2);
    ui->camera->setLayout(cameraLayout);
}


void MainWindow::mouseDoubleClickEvent(QMouseEvent *e)
{

    qDebug()<<"双击了鼠标";
}

void MainWindow::mousePressEvent(QMouseEvent *e)
{
    qDebug()<<"按下鼠标左键";

}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    qDebug()<<"鼠标移动";
}


MainWindow::~MainWindow()
{
    delete ui;
}

/*以下是tabwidget里面spinbox改变后自动调用的函数*/
//参数arg1是改变后的值
void MainWindow::on_object_position_x_spinbox_valueChanged(double arg1)
{
   qDebug()<<arg1;
    if(ui->object_number_comboBox->currentIndex()>0)
    {
        glWidget->scene.objects.at(ui->object_number_comboBox->currentIndex()-1)
                ->model.setPosition(QVector3D(ui->object_position_x_spinbox->value(),
                                              ui->object_position_y_spinbox->value(),
                                              ui->object_position_z_spinbox->value()));
        glWidget->update();
    }

}


void MainWindow::on_object_position_y_spinbox_valueChanged(double arg1)
{
    qDebug()<<arg1;
    if(ui->object_number_comboBox->currentIndex()>0)
    {
        glWidget->scene.objects.at(ui->object_number_comboBox->currentIndex()-1)
                ->model.setPosition(QVector3D(ui->object_position_x_spinbox->value(),
                                              ui->object_position_y_spinbox->value(),
                                              ui->object_position_z_spinbox->value()));
        glWidget->update();
    }
}


void MainWindow::on_object_position_z_spinbox_valueChanged(double arg1)
{
    qDebug()<<arg1;
    if(ui->object_number_comboBox->currentIndex()>0)
    {
        glWidget->scene.objects.at(ui->object_number_comboBox->currentIndex()-1)
                ->model.setPosition(QVector3D(ui->object_position_x_spinbox->value(),
                                              ui->object_position_y_spinbox->value(),
                                              ui->object_position_z_spinbox->value()));
        glWidget->update();
    }
}

void MainWindow::on_object_rotation_x_spinbox_valueChanged(double arg1)
{
    //qDebug()<<arg1;
    if(ui->object_number_comboBox->currentIndex()>0)
    {
        glWidget->scene.objects.at(ui->object_number_comboBox->currentIndex()-1)
                ->model.setRotate(QVector3D(ui->object_rotation_x_spinbox->value(),
                                              ui->object_rotation_y_spinbox->value(),
                                              ui->object_rotation_z_spinbox->value()));
        glWidget->update();
    }
}

void MainWindow::on_object_rotation_y_spinbox_valueChanged(double arg1)
{
    //qDebug()<<arg1;
    if(ui->object_number_comboBox->currentIndex()>0)
    {
        glWidget->scene.objects.at(ui->object_number_comboBox->currentIndex()-1)
                ->model.setRotate(QVector3D(ui->object_rotation_x_spinbox->value(),
                                              ui->object_rotation_y_spinbox->value(),
                                              ui->object_rotation_z_spinbox->value()));
        glWidget->update();
    }
}


void MainWindow::on_object_rotation_z_spinbox_valueChanged(double arg1)
{
    //qDebug()<<arg1;
    if(ui->object_number_comboBox->currentIndex()>0)
    {
        glWidget->scene.objects.at(ui->object_number_comboBox->currentIndex()-1)
                ->model.setRotate(QVector3D(ui->object_rotation_x_spinbox->value(),
                                              ui->object_rotation_y_spinbox->value(),
                                              ui->object_rotation_z_spinbox->value()));
        glWidget->update();
    }
}

void MainWindow::on_object_scale_x_spinbox_valueChanged(double arg1)
{
    //qDebug()<<arg1;
    if(ui->object_number_comboBox->currentIndex()>0)
    {
        glWidget->scene.objects.at(ui->object_number_comboBox->currentIndex()-1)
                ->model.setScale(QVector3D(ui->object_scale_x_spinbox->value(),
                                              ui->object_scale_y_spinbox->value(),
                                              ui->object_scale_z_spinbox->value()));
        glWidget->update();
    }
}


void MainWindow::on_object_scale_y_spinbox_valueChanged(double arg1)
{
    //qDebug()<<arg1;
    if(ui->object_number_comboBox->currentIndex()>0)
    {
        glWidget->scene.objects.at(ui->object_number_comboBox->currentIndex()-1)
                ->model.setScale(QVector3D(ui->object_scale_x_spinbox->value(),
                                              ui->object_scale_y_spinbox->value(),
                                              ui->object_scale_z_spinbox->value()));
        glWidget->update();
    }
}


void MainWindow::on_object_scale_z_spinbox_valueChanged(double arg1)
{
    //qDebug()<<arg1;
    if(ui->object_number_comboBox->currentIndex()>0)
    {
        glWidget->scene.objects.at(ui->object_number_comboBox->currentIndex()-1)
                ->model.setScale(QVector3D(ui->object_scale_x_spinbox->value(),
                                              ui->object_scale_y_spinbox->value(),
                                              ui->object_scale_z_spinbox->value()));
        glWidget->update();
    }
}


void MainWindow::on_object_color_red_spinbox_valueChanged(int R)
{
    if(glWidget->objectNumber){
        glWidget->scene.objects.at(glWidget->objectNumber-1)->color[0]=float(R)/255;
    }
    glWidget->update();
}


void MainWindow::on_object_color_green_spinbox_valueChanged(int G)
{
    if(glWidget->objectNumber){
        glWidget->scene.objects.at(glWidget->objectNumber-1)->color[1]=float(G)/255;
    }
    glWidget->update();
}


void MainWindow::on_object_color_blue_spinbox_valueChanged(int B)
{
    if(glWidget->objectNumber){
        glWidget->scene.objects.at(glWidget->objectNumber-1)->color[2]=float(B)/255;
    }
    glWidget->update();
}


void MainWindow::on_camera_position_x_spinbox_valueChanged(double arg1)
{
    //qDebug()<<arg1;
    glWidget->setXCameraPosi(arg1);
}


void MainWindow::on_camera_position_y_spinbox_valueChanged(double arg1)
{
    qDebug()<<arg1;
    glWidget->setYCameraPosi(arg1);
}


void MainWindow::on_camera_position_z_spinbox_valueChanged(double arg1)
{
    //qDebug()<<arg1;
    glWidget->setZCameraPosi(arg1);
}


void MainWindow::on_camera_focus_x_spinbox_valueChanged(double arg1)
{
    //qDebug()<<arg1;
    glWidget->setXCameraFocus(arg1);
}


void MainWindow::on_camera_focus_y_spinbox_valueChanged(double arg1)
{
    //qDebug()<<arg1;
    glWidget->setYCameraFocus(arg1);
}


void MainWindow::on_camera_focus_z_spinbox_valueChanged(double arg1)
{
    //qDebug()<<arg1;
    glWidget->setZCameraFocus(arg1);
}


void MainWindow::on_camera_cameraup_front_spinbox_valueChanged(double arg1)
{
    //qDebug()<<arg1;
}


void MainWindow::on_camera_cameraup_up_spinbox_valueChanged(double arg1)
{
    //qDebug()<<arg1;
}


void MainWindow::on_camera_cameraup_right_spinbox_valueChanged(double arg1)
{
    //qDebug()<<arg1;
}


void MainWindow::on_object_rotation_x_radioButton_clicked()
{
    glWidget->setXObjRotationSelected(true);
    glWidget->setYObjRotationSelected(false);
    glWidget->setZObjRotationSelected(false);
}


void MainWindow::on_object_rotation_y_radioButton_clicked()
{
    glWidget->setXObjRotationSelected(false);
    glWidget->setYObjRotationSelected(true);
    glWidget->setZObjRotationSelected(false);
}


void MainWindow::on_object_rotation_z_radioButton_clicked()
{
    glWidget->setXObjRotationSelected(false);
    glWidget->setYObjRotationSelected(false);
    glWidget->setZObjRotationSelected(true);
}


void MainWindow::on_tabWidget_currentChanged(int index)
{
    qDebug()<<index;
    glWidget->setCurrentIndex(index);
}


void MainWindow::on_object_number_comboBox_currentIndexChanged(int index)
{
    glWidget->setObjectNumber(index);
    emit glWidget->objectNumberChanged(index);
    if(index>0)
    {
        glWidget->objectChangEmitSignal();
    }
    glWidget->setFocus();

}

void MainWindow::on_object_renderMethod_comboBox_currentIndexChanged(int index)
{
    if(index==2||index==3)
    {
        qDebug()<<"combobox:"<<index;
        ui->object_color_widget->setHidden(false);
    }
    else
    {
        ui->object_color_widget->setHidden(true);
    }
    glWidget->setCurrentObjectShader(index);

}

void MainWindow::on_object_is_light_chekBox_stateChanged(int arg1)
{
    glWidget->setCurrentObjectEmit(arg1);
}

void MainWindow::on_hasParallel_stateChanged(int state)
{
    glWidget->scene.dirlight->dirLightActivated = state;
    glWidget->update();
}


void MainWindow::on_light_direction_x_valueChanged(double x)
{
    glWidget->scene.dirlight->setDirectionX(x);
    glWidget->update();
}


void MainWindow::on_light_direction_y_valueChanged(double y)
{
    glWidget->scene.dirlight->setDirectionY(y);
    glWidget->update();
}


void MainWindow::on_light_direction_z_valueChanged(double z)
{
    glWidget->scene.dirlight->setDirectionZ(z);
    glWidget->update();
}


void MainWindow::on_dirlight_color_red_valueChanged(int R)
{
    glWidget->scene.dirlight->setColorR(float(R)/255);
    glWidget->update();
}


void MainWindow::on_dirlight_color_green_valueChanged(int G)
{
    glWidget->scene.dirlight->setColorG(float(G)/255);
    glWidget->update();
}


void MainWindow::on_dirlight_color_blue_valueChanged(int B)
{
    glWidget->scene.dirlight->setColorB(float(B)/255);
    glWidget->update();
}


void MainWindow::on_DirLightAmbient_valueChanged(int value)
{
    DirLight::ambient = float(value)*0.01;
    glWidget->update();
}


void MainWindow::on_DirLightDiffuse_valueChanged(int value)
{
    DirLight::diffuse = float(value)*0.01;
    glWidget->update();
}


void MainWindow::on_DirLightSpecular_valueChanged(int value)
{
    DirLight::specular = float(value)*0.01;
    glWidget->update();
}


void MainWindow::on_linear_valueChanged(double linear)
{
    PointLight::linear = linear*0.001;
    glWidget->update();
}


void MainWindow::on_quadratic_valueChanged(double quadratic)
{
    PointLight::quadratic = quadratic*0.001;
    glWidget->update();
}


void MainWindow::on_pointLightAmbient_valueChanged(int ambient)
{
    PointLight::ambient = float(ambient)*0.01;
    glWidget->update();
}


void MainWindow::on_pointLightDiffuse_valueChanged(int diffuse)
{
    PointLight::diffuse = float(diffuse)*0.01;
    glWidget->update();
}


void MainWindow::on_pointLightSpecular_valueChanged(int specular)
{
    PointLight::specular = float(specular)*0.01;
    glWidget->update();
}


void MainWindow::on_light_parallel_checkbox_stateChanged(int arg1)
{
    glWidget->shadowShow = (arg1!=0);
    glWidget->update();
}

