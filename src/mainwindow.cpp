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

