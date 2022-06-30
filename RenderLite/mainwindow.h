#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMouseEvent>
#include "glwidget.h"
#include "camera.h"


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

    //static void setCamera(camera glcamera);


protected:
    virtual void mouseDoubleClickEvent(QMouseEvent *e);
    virtual void mousePressEvent(QMouseEvent *e);
     virtual void mouseMoveEvent(QMouseEvent * event);

    
private:
    Ui::MainWindow *ui;
    GLWidget *glWidget;
    QWidget *mainWidget;
    QWidget *addlightwidget;

    int objectNumSize=1;

signals:
    void widgetSizeChanged();
    void objectSizeChanged(int theObjectSize);

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
    void on_object_rotation_x_radioButton_clicked();
    void on_object_rotation_y_radioButton_clicked();
    void on_object_rotation_z_radioButton_clicked();
    void on_tabWidget_currentChanged(int index);
    void on_object_number_comboBox_currentIndexChanged(int index);
    void on_light_addpointolite_pushbutton_clicked();
    void on_light_parallel_checkbox_clicked();
    void on_light_direction_x_spinbox_valueChanged(double arg1);
    void on_light_color_red_spinbox_valueChanged(int arg1);
    void on_light_direction_y_spinbox_valueChanged(double arg1);
    void on_light_direction_z_spinbox_valueChanged(double arg1);
    void on_light_color_green_spinbox_valueChanged(int arg1);
    void on_light_color_blue_spinbox_valueChanged(int arg1);
    void on_light_parallel_checkbox_stateChanged(int arg1);
    void on_horizontalSlider_valueChanged(int value);
    void on_horizontalSlider_2_valueChanged(int value);
    void on_horizontalSlider_3_valueChanged(int value);
    void on_doubleSpinBox_4_valueChanged(double arg1);
    void on_doubleSpinBox_5_valueChanged(double arg1);
    void on_horizontalSlider_4_valueChanged(int value);
    void on_horizontalSlider_5_valueChanged(int value);
    void on_horizontalSlider_6_valueChanged(int value);
    void on_checkBox_stateChanged(int arg1);};
#endif // MAINWINDOW_H
