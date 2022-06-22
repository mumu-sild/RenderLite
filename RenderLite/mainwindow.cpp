#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDesktopWidget>
#include <QApplication>
#include <QAction>
#include <QHBoxLayout>
#include <QWidget>
#include <QTabWidget>
#include <QDebug>



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    setUiStyle();
}

void MainWindow::setUiStyle()
{
    //设置各个窗口部件大小和位置
    //设置整个窗口和屏幕一样大,并移动位置
    this->resize(QApplication::desktop()->width()*0.9,
                 QApplication::desktop()->height()*0.9);
    this->move(QApplication::desktop()->width()*0.05,
               QApplication::desktop()->height()*0.02);

    mainWidget = new QWidget;
    setCentralWidget(mainWidget);

    QHBoxLayout *mainLayout = new QHBoxLayout;


    glWidget = new GLWidget;

    //ui->tabWidget->setTabText(0,tr("物体"));
    //ui->tabWidget->setTabText(1,tr("摄像机"));
    //ui->tabWidget->setTabText(2,tr("灯光"));
    ui->tabWidget->setMaximumSize(this->width()*0.18,this->height());
    mainLayout->addWidget(glWidget);
    mainLayout->addWidget(ui->tabWidget);

    mainWidget->setLayout(mainLayout);

    ui->position_x_spinbox->setRange(-100,100);
    ui->position_y_spinbox->setRange(-100,100);
    ui->position_z_spinbox->setRange(-100,100);

   // ui->tabWidget->setMaximumWidth(mainWidget->width()*0.15);
//    connect(this,&MainWindow::widgetSizeChanged,[=](){
//        tabwidget->setMaximumWidth(mainWidget->width()*0.15);
//    });

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

