//#pragma once

//#include <QMainWindow>
//#include <QCheckBox>
//#include <QSlider>
//#include <QSpinBox>
//#include <QDoubleSpinBox>
//#include <QPushButton>
//#include "realtime.h"

//class MainWindow : public QWidget
//{
//    Q_OBJECT

//public:
//    void initialize();
//    void finish();

//private:
//    void connectUIElements();
//    void connectParam1();
//    void connectTrans();
//    void connectScaling();
//    void connectNoiseTrans();
//    void connectNoiseScaling();
//    void connectStep();
//    void connectNumCells();
//    void connectInvertDensity();
//    void connectGammaCorrect();
//    void connectUploadFile();
//    void connectExtraCredit();

//    Realtime *realtime;
//    QCheckBox *invertDensity;
//    QCheckBox *gammaCorrect;
//    QPushButton *uploadFile;

//    // Density multiplier
//    QSlider *p1Slider;
//    QDoubleSpinBox *p1Box;

//    // Step size
//    QSlider *stepSlider;
//    QDoubleSpinBox *stepBox;

//    // #cells per axis
//    QSlider *cellsFineSlider, *cellsCoarseSlider;
//    QSpinBox *cellsFineBox, *cellsCoarseBox;

//    // Volume transforms
//    QSlider *transxSlider, *transySlider, *transzSlider;
//    QDoubleSpinBox *transxBox, *transyBox, *transzBox;
//    QSlider *scalexSlider, *scaleySlider, *scalezSlider;
//    QDoubleSpinBox *scalexBox, *scaleyBox, *scalezBox;

//    // Noise transforms
//    QSlider *noiseTransxSlider, *noiseTransySlider, *noiseTranszSlider;
//    QDoubleSpinBox *noiseTransxBox, *noiseTransyBox, *noiseTranszBox;
//    QSlider *noiseScaleSlider;
//    QDoubleSpinBox *noiseScaleBox;

//    // Extra Credit:
//    QCheckBox *ec1;
//    QCheckBox *ec2;
//    QCheckBox *ec3;
//    QCheckBox *ec4;

//private slots:
//    void onInvertDensity();
//    void onGammaCorrect();
//    void onUploadFile();

//    void onValChangeCellsFine(int newValue);
//    void onValChangeCellsCoarse(int newValue);

//    void onValChangeP1Slider(int newValue);
//    void onValChangeStepSlider(int newValue);
//    void onValChangeTransxSlider(int newValue);
//    void onValChangeTransySlider(int newValue);
//    void onValChangeTranszSlider(int newValue);
//    void onValChangeNoiseTransxSlider(int newValue);
//    void onValChangeNoiseTransySlider(int newValue);
//    void onValChangeNoiseTranszSlider(int newValue);
//    void onValChangeNoiseScaleSlider(int newValue);
//    void onValChangeScalexSlider(int newValue);
//    void onValChangeScaleySlider(int newValue);
//    void onValChangeScalezSlider(int newValue);


//    void onValChangeP1Box(double newValue);
//    void onValChangeStepBox(double newValue);
//    void onValChangeTransxBox(double newValue);
//    void onValChangeTransyBox(double newValue);
//    void onValChangeTranszBox(double newValue);
//    void onValChangeNoiseTransxBox(double newValue);
//    void onValChangeNoiseTransyBox(double newValue);
//    void onValChangeNoiseTranszBox(double newValue);
//    void onValChangeNoiseScaleBox(double newValue);
//    void onValChangeScalexBox(double newValue);
//    void onValChangeScaleyBox(double newValue);
//    void onValChangeScalezBox(double newValue);

//    // Extra Credit:
//    void onExtraCredit1();
//    void onExtraCredit2();
//    void onExtraCredit3();
//    void onExtraCredit4();
//};

#include "realtime.h"
#include "terrain/glwidget.h"

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

    void on_noiseScaling_horizontalSlider_sliderMoved(int position);

    void on_noiseScaling_doubleSpinBox_valueChanged(double arg1);

    void on_invert_checkBox_stateChanged(int arg1);

    void on_gamma_checkBox_stateChanged(int arg1);

    void on_lightLatitude_horizontalSlider_sliderMoved(int position);

    void on_lightLongitude_horizontalSlider_sliderMoved(int position);

    void on_lightLatitude_doubleSpinBox_valueChanged(double arg1);

    void on_lightLongitude_doubleSpinBox_valueChanged(double arg1);

    void on_detailTextureTrans1_doubleSpinBox_valueChanged(double arg1);

    void on_detailTextureTrans2_doubleSpinBox_valueChanged(double arg1);

    void on_detailTextureTrans3_doubleSpinBox_valueChanged(double arg1);

    void on_detailTexture_fineR_spinBox_valueChanged(int arg1);

    void on_detailTexture_mediumR_spinBox_valueChanged(int arg1);

    void on_detailTexture_coarseR_spinBox_valueChanged(int arg1);

    void on_detailTexture_weightR_spinBox_valueChanged(double arg1);

    void on_detailTexture_fineG_spinBox_valueChanged(int arg1);

    void on_detailTexture_mediumG_spinBox_valueChanged(int arg1);

    void on_detailTexture_coarseG_spinBox_valueChanged(int arg1);

    void on_detailTexture_weightG_spinBox_valueChanged(double arg1);

    void on_detailTexture_fineB_spinBox_valueChanged(int arg1);

    void on_detailTexture_mediumB_spinBox_valueChanged(int arg1);

    void on_detailTexture_coarseB_spinBox_valueChanged(int arg1);

    void on_detailTexture_weightB_spinBox_valueChanged(double arg1);

    void on_detailTexture_fineA_spinBox_valueChanged(int arg1);

    void on_detailTexture_mediumA_spinBox_valueChanged(int arg1);

    void on_detailTexture_coarseA_spinBox_valueChanged(int arg1);

    void on_detailTexture_weightA_spinBox_valueChanged(double arg1);

    void on_shapeTextureTrans1_doubleSpinBox_valueChanged(double arg1);

    void on_shapeTextureTrans2_doubleSpinBox_valueChanged(double arg1);

    void on_shapeTextureTrans3_doubleSpinBox_valueChanged(double arg1);

    void on_shapeTexture_fineR_spinBox_valueChanged(int arg1);

    void on_shapeTexture_mediumR_spinBox_valueChanged(int arg1);

    void on_shapeTexture_coarseR_spinBox_valueChanged(int arg1);

    void on_shapeTexture_weightR_doubleSpinBox_valueChanged(double arg1);

    void on_shapeTexture_fineG_spinBox_valueChanged(int arg1);

    void on_shapeTexture_mediumG_spinBox_valueChanged(int arg1);

    void on_shapeTexture_coarseG_spinBox_valueChanged(int arg1);

    void on_shapeTexture_weightG_doubleSpinBox_valueChanged(double arg1);

    void on_shapeTexture_fineB_spinBox_valueChanged(int arg1);

    void on_shapeTexture_mediumB_spinBox_valueChanged(int arg1);

    void on_shapeTexture_coarseB_spinBox_valueChanged(int arg1);

    void on_shapeTexture_weightB_doubleSpinBox_valueChanged(double arg1);

    void on_shapeTexture_fineA_spinBox_valueChanged(int arg1);

    void on_shapeTexture_mediumA_spinBox_valueChanged(int arg1);

    void on_shapeTexture_coarseA_spinBox_valueChanged(int arg1);

    void on_shapeTexture_weightA_doubleSpinBox_valueChanged(double arg1);

    void on_lightColor_R_doubleSpinBox_valueChanged(double arg1);

    void on_lightColor_G_doubleSpinBox_valueChanged(double arg1);

    void on_lightColor_B_doubleSpinBox_valueChanged(double arg1);

private:
    Ui::MainWindow *ui;
    Realtime *realtime;
};
#endif // MAINWINDOW_H
