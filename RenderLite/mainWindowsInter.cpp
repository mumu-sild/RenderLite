#include "mainwindow.h"

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
