#include "realtime.h"

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_densityMultiplier_valueChanged(double arg1);

    void on_stepNumber_spinBox_valueChanged(int arg1);

    void on_stepSize_spinBox_valueChanged(int arg1);

    void on_lightAborspMulti_doubleSpinBox_valueChanged(double arg1);

    void on_lightTrans_doubleSpinBox_valueChanged(double arg1);

    void on_valScaling1_doubleSpinBox_valueChanged(double arg1);

    void on_valScaling2_doubleSpinBox_valueChanged(double arg1);

    void on_valScaling3_doubleSpinBox_valueChanged(double arg1);

private:
    Ui::MainWindow *ui;
    Realtime *realtime;
};
#endif // MAINWINDOW_H
