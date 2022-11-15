#pragma once

#include <QMainWindow>
#include <QCheckBox>
#include <QSlider>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include "realtime.h"

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    void initialize();
    void finish();

private:
    void connectUIElements();
    void connectParam1();
    void connectNear();
    void connectFar();
    void connectPerPixelFilter();
    void connectKernelBasedFilter();
    void connectUploadFile();
    void connectExtraCredit();

    Realtime *realtime;
    QCheckBox *filter1;
    QCheckBox *filter2;
    QPushButton *uploadFile;
    QSlider *p1Slider;
    QDoubleSpinBox *p1Box;
    QSlider *nearSlider;
    QSlider *farSlider;
    QDoubleSpinBox *nearBox;
    QDoubleSpinBox *farBox;

    // Extra Credit:
    QCheckBox *ec1;
    QCheckBox *ec2;
    QCheckBox *ec3;
    QCheckBox *ec4;

private slots:
    void onPerPixelFilter();
    void onKernelBasedFilter();
    void onUploadFile();

    void onValChangeP1Slider(int newValue);
    void onValChangeNearSlider(int newValue);
    void onValChangeFarSlider(int newValue);

    void onValChangeP1Box(double newValue);
    void onValChangeNearBox(double newValue);
    void onValChangeFarBox(double newValue);

    // Extra Credit:
    void onExtraCredit1();
    void onExtraCredit2();
    void onExtraCredit3();
    void onExtraCredit4();
};
