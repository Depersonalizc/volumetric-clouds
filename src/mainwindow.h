#pragma once

#include <QMainWindow>
#include <QCheckBox>
#include <QSlider>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include "realtime.h"
#include "terrain/glwidget.h"

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    void initialize();
    void finish();

private:
    void connectUIElements();
    void connectParam1();
    void connectTrans();
    void connectScaling();
    void connectNoiseTrans();
    void connectNoiseScaling();
    void connectStep();
    void connectNumCells();
    void connectInvertDensity();
    void connectGammaCorrect();
    void connectUploadFile();
    void connectExtraCredit();

    GLWidget *glWidget;

    Realtime *realtime;
    QCheckBox *invertDensity;
    QCheckBox *gammaCorrect;
    QPushButton *uploadFile;

    // Density multiplier
    QSlider *p1Slider;
    QDoubleSpinBox *p1Box;

    // Step size
    QSlider *stepSlider;
    QDoubleSpinBox *stepBox;

    // #cells per axis
    QSlider *cellsFineSlider, *cellsCoarseSlider;
    QSpinBox *cellsFineBox, *cellsCoarseBox;

    // Volume transforms
    QSlider *transxSlider, *transySlider, *transzSlider;
    QDoubleSpinBox *transxBox, *transyBox, *transzBox;
    QSlider *scalexSlider, *scaleySlider, *scalezSlider;
    QDoubleSpinBox *scalexBox, *scaleyBox, *scalezBox;

    // Noise transforms
    QSlider *noiseTransxSlider, *noiseTransySlider, *noiseTranszSlider;
    QDoubleSpinBox *noiseTransxBox, *noiseTransyBox, *noiseTranszBox;
    QSlider *noiseScaleSlider;
    QDoubleSpinBox *noiseScaleBox;

    // Extra Credit:
    QCheckBox *ec1;
    QCheckBox *ec2;
    QCheckBox *ec3;
    QCheckBox *ec4;

private slots:
    void onInvertDensity();
    void onGammaCorrect();
    void onUploadFile();

    void onValChangeCellsFine(int newValue);
    void onValChangeCellsCoarse(int newValue);

    void onValChangeP1Slider(int newValue);
    void onValChangeStepSlider(int newValue);
    void onValChangeTransxSlider(int newValue);
    void onValChangeTransySlider(int newValue);
    void onValChangeTranszSlider(int newValue);
    void onValChangeNoiseTransxSlider(int newValue);
    void onValChangeNoiseTransySlider(int newValue);
    void onValChangeNoiseTranszSlider(int newValue);
    void onValChangeNoiseScaleSlider(int newValue);
    void onValChangeScalexSlider(int newValue);
    void onValChangeScaleySlider(int newValue);
    void onValChangeScalezSlider(int newValue);


    void onValChangeP1Box(double newValue);
    void onValChangeStepBox(double newValue);
    void onValChangeTransxBox(double newValue);
    void onValChangeTransyBox(double newValue);
    void onValChangeTranszBox(double newValue);
    void onValChangeNoiseTransxBox(double newValue);
    void onValChangeNoiseTransyBox(double newValue);
    void onValChangeNoiseTranszBox(double newValue);
    void onValChangeNoiseScaleBox(double newValue);
    void onValChangeScalexBox(double newValue);
    void onValChangeScaleyBox(double newValue);
    void onValChangeScalezBox(double newValue);

    // Extra Credit:
    void onExtraCredit1();
    void onExtraCredit2();
    void onExtraCredit3();
    void onExtraCredit4();
};
