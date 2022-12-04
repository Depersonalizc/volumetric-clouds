#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    realtime = new Realtime;
    ui->setupUi(this);
    ui->screen->addWidget(realtime, 1);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_densityMultiplier_valueChanged(double arg1)
{
    ui->densityMultiplier->setValue(arg1);
    settings.densityMult = ui->densityMultiplier->value();
    realtime->settingsChanged();
}

void MainWindow::on_stepNumber_spinBox_valueChanged(int arg1)
{
    ui->stepNumber_spinBox->setValue(arg1);
    settings.numSteps = ui->stepNumber_spinBox->value();
    realtime->settingsChanged();
}

void MainWindow::on_stepSize_spinBox_valueChanged(int arg1)
{
    ui->stepSize_spinBox->setValue(arg1);
    settings.stepSize = ui->stepSize_spinBox->value();
    realtime->settingsChanged();
}


void MainWindow::on_lightAborspMulti_doubleSpinBox_valueChanged(double arg1)
{
    ui->lightAborspMulti_doubleSpinBox->setValue(arg1);
   // settings. = ui->stepSize_spinBox->value();
    realtime->settingsChanged();
}


void MainWindow::on_lightTrans_doubleSpinBox_valueChanged(double arg1)
{
    ui->lightTrans_doubleSpinBox->setValue(arg1);
   // settings.
    realtime->settingsChanged();
}


void MainWindow::on_valScaling1_doubleSpinBox_valueChanged(double arg1)
{
    ui->valScaling1_doubleSpinBox->setValue(arg1);
    settings.volumeScaling[1] = ui->valScaling1_doubleSpinBox->value();
    realtime->settingsChanged();
}


void MainWindow::on_valScaling2_doubleSpinBox_valueChanged(double arg1)
{
    ui->valScaling2_doubleSpinBox->setValue(arg1);
    settings.volumeScaling[2] = ui->valScaling2_doubleSpinBox->value();
    realtime->settingsChanged();
}


void MainWindow::on_valScaling3_doubleSpinBox_valueChanged(double arg1)
{
    ui->valScaling3_doubleSpinBox->setValue(arg1);
    settings.volumeScaling[3] = ui->valScaling3_doubleSpinBox->value();
    realtime->settingsChanged();
}

