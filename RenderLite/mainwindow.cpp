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



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    glWidget = new GLWidget;
    setUiStyle();
    this->showMaximized();

    //点击“导入”的动作打开文件
    connect(ui->import_file,&QAction::triggered,[=](){
        QFileDialog selectFile(this);
        if(selectFile.exec()==0)
            return;
        QStringList filePaths = selectFile.selectedFiles();
        //theFilePath为所要的路径
        QString theFilePath = filePaths[0];
        qDebug()<<theFilePath;
        glWidget->importModel(theFilePath);

    });

    //导出文件为jpg格式
    connect(ui->outport_file,&QAction::triggered,[=](){
        QFileDialog outportFile(this);
        if(outportFile.exec()==0)
            return;

        QStringList outFilePaths = outportFile.selectedFiles();
        //theFilePath为所要的路径
        QString outFilePath = outFilePaths[0];
        qDebug()<<outFilePath;
        QPixmap pixmap = QPixmap::grabWidget(glWidget);
        pixmap.save(outFilePath,"JPG");
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

    ui->tabWidget->setMaximumSize(this->width()*0.18,this->height());
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

}


void MainWindow::on_object_position_y_spinbox_valueChanged(double arg1)
{
    qDebug()<<arg1;
}


void MainWindow::on_object_position_z_spinbox_valueChanged(double arg1)
{
    qDebug()<<arg1;
}


void MainWindow::on_object_rotation_x_spinbox_valueChanged(double arg1)
{
    qDebug()<<arg1;
}


void MainWindow::on_object_rotation_y_spinbox_valueChanged(double arg1)
{
    qDebug()<<arg1;
}


void MainWindow::on_object_rotation_z_spinbox_valueChanged(double arg1)
{
    qDebug()<<arg1;
}


void MainWindow::on_object_scale_x_spinbox_valueChanged(double arg1)
{
    qDebug()<<arg1;
}


void MainWindow::on_object_scale_y_spinbox_valueChanged(double arg1)
{
    qDebug()<<arg1;
}


void MainWindow::on_object_scale_z_spinbox_valueChanged(double arg1)
{
    qDebug()<<arg1;
}


void MainWindow::on_object_color_red_spinbox_valueChanged(int arg1)
{
    qDebug()<<arg1;
}


void MainWindow::on_object_color_green_spinbox_valueChanged(int arg1)
{
    qDebug()<<arg1;
}


void MainWindow::on_object_color_blue_spinbox_valueChanged(int arg1)
{
    qDebug()<<arg1;
}


void MainWindow::on_camera_position_x_spinbox_valueChanged(double arg1)
{
    qDebug()<<arg1;
    glWidget->setXCameraPosi(arg1);
}


void MainWindow::on_camera_position_y_spinbox_valueChanged(double arg1)
{
    qDebug()<<arg1;
    glWidget->setYCameraPosi(arg1);
}


void MainWindow::on_camera_position_z_spinbox_valueChanged(double arg1)
{
    qDebug()<<arg1;
    glWidget->setZCameraPosi(arg1);
}


void MainWindow::on_camera_focus_x_spinbox_valueChanged(double arg1)
{
    qDebug()<<arg1;
    glWidget->setXCameraFocus(arg1);
}


void MainWindow::on_camera_focus_y_spinbox_valueChanged(double arg1)
{
    qDebug()<<arg1;
    glWidget->setYCameraFocus(arg1);
}


void MainWindow::on_camera_focus_z_spinbox_valueChanged(double arg1)
{
    qDebug()<<arg1;
    glWidget->setZCameraFocus(arg1);
}


void MainWindow::on_camera_cameraup_front_spinbox_valueChanged(double arg1)
{
    qDebug()<<arg1;
}


void MainWindow::on_camera_cameraup_up_spinbox_valueChanged(double arg1)
{
    qDebug()<<arg1;
}


void MainWindow::on_camera_cameraup_right_spinbox_valueChanged(double arg1)
{
    qDebug()<<arg1;
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

void MainWindow::on_light_addpointolite_pushbutton_clicked()
{

}


void MainWindow::on_light_parallel_checkbox_clicked()
{

}

void MainWindow::on_light_direction_x_spinbox_valueChanged(double arg1)
{
    glWidget->setDirLightDirectionX(arg1);
}

void MainWindow::on_light_color_red_spinbox_valueChanged(int arg1)
{
    glWidget->setDirLightColorR(arg1);
}



void MainWindow::on_light_direction_y_spinbox_valueChanged(double arg1)
{
    glWidget->setDirLightDirectionY(arg1);
}


void MainWindow::on_light_direction_z_spinbox_valueChanged(double arg1)
{
    glWidget->setDirLightDirectionZ(arg1);
}


void MainWindow::on_light_color_green_spinbox_valueChanged(int arg1)
{
    glWidget->setDirLightColorG(arg1);
}


void MainWindow::on_light_color_blue_spinbox_valueChanged(int arg1)
{
    glWidget->setDirLightColorB(arg1);
}


void MainWindow::on_light_parallel_checkbox_stateChanged(int arg1)
{
    glWidget->setPointLightActivated(arg1);
}


void MainWindow::on_horizontalSlider_valueChanged(int value)
{
    glWidget->setDirAmbient(value/100.0f);
}


void MainWindow::on_horizontalSlider_2_valueChanged(int value)
{
    glWidget->setDirDiffuse(value/100.0f);
}


void MainWindow::on_horizontalSlider_3_valueChanged(int value)
{
    glWidget->setDirSpecular(value/100.0f);
}


void MainWindow::on_doubleSpinBox_4_valueChanged(double arg1)
{
    glWidget->setLinear(arg1);
}


void MainWindow::on_doubleSpinBox_5_valueChanged(double arg1)
{
    glWidget->setQuadratic(arg1);
}


void MainWindow::on_horizontalSlider_4_valueChanged(int value)
{
    glWidget->setPointAmbient(value/100.0f);
}


void MainWindow::on_horizontalSlider_5_valueChanged(int value)
{
    glWidget->setPointDiffuse(value/100.0f);
}


void MainWindow::on_horizontalSlider_6_valueChanged(int value)
{
    glWidget->setPointSpecular(value/100.0f);
}


void MainWindow::on_checkBox_stateChanged(int arg1)
{
    glWidget->setDirLightActivated(arg1);
}

