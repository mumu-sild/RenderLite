#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMouseEvent>
#include "glwidget.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setUiStyle();

protected:
    virtual void mouseDoubleClickEvent(QMouseEvent *e);
    virtual void mousePressEvent(QMouseEvent *e);
     virtual void mouseMoveEvent(QMouseEvent * event);

    
private:
    Ui::MainWindow *ui;
    GLWidget *glWidget;
    QWidget *mainWidget;

signals:
    void widgetSizeChanged();

private slots:
    void on_object_position_x_spinbox_valueChanged(double arg1);
    void on_object_position_y_spinbox_valueChanged(double arg1);
    void on_object_position_z_spinbox_valueChanged(double arg1);
    void on_object_rotation_x_spinbox_valueChanged(double arg1);
    void on_object_rotation_y_spinbox_valueChanged(double arg1);
    void on_object_rotation_z_spinbox_valueChanged(double arg1);
    void on_object_scale_x_spinbox_valueChanged(double arg1);
    void on_object_scale_y_spinbox_valueChanged(double arg1);
    void on_object_scale_z_spinbox_valueChanged(double arg1);
    void on_object_color_red_spinbox_valueChanged(int arg1);
    void on_object_color_green_spinbox_valueChanged(int arg1);
    void on_object_color_blue_spinbox_valueChanged(int arg1);
    void on_camera_position_x_spinbox_valueChanged(double arg1);
    void on_camera_position_y_spinbox_valueChanged(double arg1);
    void on_camera_position_z_spinbox_valueChanged(double arg1);
    void on_camera_focus_x_spinbox_valueChanged(double arg1);
    void on_camera_focus_y_spinbox_valueChanged(double arg1);
    void on_camera_focus_z_spinbox_valueChanged(double arg1);
    void on_camera_cameraup_front_spinbox_valueChanged(double arg1);
    void on_camera_cameraup_up_spinbox_valueChanged(double arg1);
    void on_camera_cameraup_right_spinbox_valueChanged(double arg1);
};
#endif // MAINWINDOW_H
