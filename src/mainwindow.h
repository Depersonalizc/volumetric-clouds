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

    void on_densityWeight_doubleSpinBox_valueChanged(double arg1);

    void on_densityOffset_doubleSpinBox_valueChanged(double arg1);

    void on_highResScaling_R_doubleSpinBox_valueChanged(double arg1);

    void on_highResScaling_G_doubleSpinBox_valueChanged(double arg1);

    void on_highResScaling_B_doubleSpinBox_valueChanged(double arg1);

    void on_highResScaling_A_doubleSpinBox_valueChanged(double arg1);

    void on_lowResScaling_doubleSpinBox_valueChanged(double arg1);

    void on_volTrans_x_doubleSpinBox_valueChanged(double arg1);

    void on_volTrans_y_doubleSpinBox_valueChanged(double arg1);

    void on_volTrans_z_doubleSpinBox_valueChanged(double arg1);

private:
    Ui::MainWindow *ui;
    Realtime *realtime;
};
#endif // MAINWINDOW_H
