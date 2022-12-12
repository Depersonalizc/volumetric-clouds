
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
    std::cout << "change step" << settings.numSteps;
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
    settings.cloudLightAbsorptionMult = ui->lightAborspMulti_doubleSpinBox->value();
    realtime->settingsChanged();
}


void MainWindow::on_lightTrans_doubleSpinBox_valueChanged(double arg1)
{
    ui->lightTrans_doubleSpinBox->setValue(arg1);
    settings.minLightTransmittance = ui->lightTrans_doubleSpinBox->value();
    realtime->settingsChanged();
}


void MainWindow::on_valScaling1_doubleSpinBox_valueChanged(double arg1)
{
    ui->valScaling1_doubleSpinBox->setValue(arg1);
    settings.volumeScaling[0] = ui->valScaling1_doubleSpinBox->value();
    realtime->settingsChanged();
}


void MainWindow::on_valScaling2_doubleSpinBox_valueChanged(double arg1)
{
    ui->valScaling2_doubleSpinBox->setValue(arg1);
    settings.volumeScaling[1] = ui->valScaling2_doubleSpinBox->value();
    realtime->settingsChanged();
}


void MainWindow::on_valScaling3_doubleSpinBox_valueChanged(double arg1)
{
    ui->valScaling3_doubleSpinBox->setValue(arg1);
    settings.volumeScaling[2] = ui->valScaling3_doubleSpinBox->value();
    realtime->settingsChanged();
}


void MainWindow::on_invert_checkBox_stateChanged(int arg1)
{
    settings.invertDensity = !settings.invertDensity;
    realtime->settingsChanged();
}


void MainWindow::on_gamma_checkBox_stateChanged(int arg1)
{
    settings.gammaCorrect = !settings.gammaCorrect;
    realtime->settingsChanged();
}



void MainWindow::on_lightLatitude_horizontalSlider_sliderMoved(int position)
{
    ui->lightLatitude_horizontalSlider->setValue(position);
    ui->lightLatitude_doubleSpinBox->setValue(position);

    settings.lightData.latitude = position;
    realtime->settingsChanged();
}


void MainWindow::on_lightLongitude_horizontalSlider_sliderMoved(int position)
{
    ui->lightLongitude_horizontalSlider->setValue(position);
    ui->lightLongitude_doubleSpinBox->setValue(position);
    settings.lightData.longitude = position;
    realtime->settingsChanged();
}


void MainWindow::on_lightLatitude_doubleSpinBox_valueChanged(double arg1)
{
    ui->lightLatitude_doubleSpinBox->setValue(arg1);
    ui->lightLatitude_horizontalSlider->setValue(arg1);
    ///TO:DO change appropriate setting in settings.
    realtime->settingsChanged();
}


void MainWindow::on_lightLongitude_doubleSpinBox_valueChanged(double arg1)
{
    ui->lightLongitude_doubleSpinBox->setValue(arg1);
    ui->lightLongitude_horizontalSlider->setValue(arg1);
    ///TO:DO change appropriate setting in settings.
    realtime->settingsChanged();
}


void MainWindow::on_detailTextureTrans1_doubleSpinBox_valueChanged(double arg1)
{
    ui->detailTextureTrans1_doubleSpinBox->setValue(arg1);
    settings.loResNoise.translate[0] = arg1;
    realtime->settingsChanged();
}


void MainWindow::on_detailTextureTrans2_doubleSpinBox_valueChanged(double arg1)
{
    ui->detailTextureTrans2_doubleSpinBox->setValue(arg1);
    settings.loResNoise.translate[1] = arg1;
    realtime->settingsChanged();
}


void MainWindow::on_detailTextureTrans3_doubleSpinBox_valueChanged(double arg1)
{
    ui->detailTextureTrans3_doubleSpinBox->setValue(arg1);
    settings.loResNoise.translate[2] = arg1;
    realtime->settingsChanged();
}


void MainWindow::on_detailTexture_fineR_spinBox_valueChanged(int arg1)
{
    ui->detailTexture_fineR_spinBox->setValue(arg1);
    settings.loResNoise.worleyPointsParams[0].cellsPerAxisFine = arg1;
    settings.newFineArray = true;
    settings.curSlot = 1;
    settings.curChannel = 0;
    realtime->settingsChanged();
}


void MainWindow::on_detailTexture_mediumR_spinBox_valueChanged(int arg1)
{
    ui->detailTexture_mediumR_spinBox->setValue(arg1);
    settings.loResNoise.worleyPointsParams[0].cellsPerAxisMedium = arg1;
    settings.newMediumArray = true;
    settings.curSlot = 1;
    settings.curChannel = 0;
    realtime->settingsChanged();
}


void MainWindow::on_detailTexture_coarseR_spinBox_valueChanged(int arg1)
{
    ui->detailTexture_coarseR_spinBox->setValue(arg1);
    settings.loResNoise.worleyPointsParams[0].cellsPerAxisCoarse = arg1;
    settings.newCoarseArray = true;
    settings.curSlot = 1;
    settings.curChannel = 0;
    realtime->settingsChanged();
}


void MainWindow::on_detailTexture_weightR_spinBox_valueChanged(double arg1)
{
    ui->detailTexture_weightR_spinBox->setValue(arg1);
    settings.loResNoise.channelWeights[0] = arg1;
    realtime->settingsChanged();
}


void MainWindow::on_detailTexture_fineG_spinBox_valueChanged(int arg1)
{
    ui->detailTexture_fineG_spinBox->setValue(arg1);
    settings.loResNoise.worleyPointsParams[1].cellsPerAxisFine = arg1;
    settings.newFineArray = true;
    settings.curSlot = 1;
    settings.curChannel = 1;
    realtime->settingsChanged();
}


void MainWindow::on_detailTexture_mediumG_spinBox_valueChanged(int arg1)
{
    ui->detailTexture_mediumG_spinBox->setValue(arg1);
    settings.newMediumArray = true;
    settings.loResNoise.worleyPointsParams[1].cellsPerAxisMedium = arg1;
    settings.curSlot = 1;
    settings.curChannel = 1;
    realtime->settingsChanged();
}


void MainWindow::on_detailTexture_coarseG_spinBox_valueChanged(int arg1)
{
    ui->detailTexture_coarseG_spinBox->setValue(arg1);
    settings.newCoarseArray = true;
    settings.loResNoise.worleyPointsParams[1].cellsPerAxisCoarse = arg1;
    settings.curSlot = 1;
    settings.curChannel = 1;
    realtime->settingsChanged();
}


void MainWindow::on_detailTexture_weightG_spinBox_valueChanged(double arg1)
{
    ui->detailTexture_weightG_spinBox->setValue(arg1);
    settings.loResNoise.channelWeights[1] = arg1;
    realtime->settingsChanged();
}


void MainWindow::on_detailTexture_fineB_spinBox_valueChanged(int arg1)
{
    ui->detailTexture_fineB_spinBox->setValue(arg1);
    settings.newFineArray = true;
    settings.loResNoise.worleyPointsParams[2].cellsPerAxisFine = arg1;
    settings.curSlot = 1;
    settings.curChannel = 2;
    realtime->settingsChanged();
}


void MainWindow::on_detailTexture_mediumB_spinBox_valueChanged(int arg1)
{
    ui->detailTexture_mediumB_spinBox->setValue(arg1);
    settings.newMediumArray = true;
    settings.loResNoise.worleyPointsParams[2].cellsPerAxisMedium = arg1;
    settings.curSlot = 1;
    settings.curChannel = 2;
    realtime->settingsChanged();
}


void MainWindow::on_detailTexture_coarseB_spinBox_valueChanged(int arg1)
{
    ui->detailTexture_coarseB_spinBox->setValue(arg1);
    settings.newCoarseArray = true;
    settings.loResNoise.worleyPointsParams[2].cellsPerAxisCoarse = arg1;
    settings.curSlot = 1;
    settings.curChannel = 2;
    realtime->settingsChanged();
}


void MainWindow::on_detailTexture_weightB_spinBox_valueChanged(double arg1)
{
    ui->detailTexture_weightB_spinBox->setValue(arg1);
    settings.loResNoise.channelWeights[2] = arg1;
    realtime->settingsChanged();
}


void MainWindow::on_detailTexture_fineA_spinBox_valueChanged(int arg1)
{
    ui->detailTexture_fineA_spinBox->setValue(arg1);
    settings.newFineArray = true;
    settings.loResNoise.worleyPointsParams[3].cellsPerAxisFine = arg1;
    settings.curSlot = 1;
    settings.curChannel = 3;
    realtime->settingsChanged();
}


void MainWindow::on_detailTexture_mediumA_spinBox_valueChanged(int arg1)
{
    ui->detailTexture_mediumA_spinBox->setValue(arg1);
    settings.newMediumArray = true;
    settings.loResNoise.worleyPointsParams[3].cellsPerAxisMedium = arg1;
    settings.curSlot = 1;
    settings.curChannel = 3;
    realtime->settingsChanged();
}


void MainWindow::on_detailTexture_coarseA_spinBox_valueChanged(int arg1)
{
    ui->detailTexture_coarseA_spinBox->setValue(arg1);
    settings.newCoarseArray = true;
    settings.loResNoise.worleyPointsParams[3].cellsPerAxisCoarse = arg1;
    settings.curSlot = 1;
    settings.curChannel = 3;
    realtime->settingsChanged();
}


void MainWindow::on_detailTexture_weightA_spinBox_valueChanged(double arg1)
{
    ui->detailTexture_weightA_spinBox->setValue(arg1);
    settings.loResNoise.channelWeights[3] = arg1;
    realtime->settingsChanged();
}

// high res
void MainWindow::on_shapeTextureTrans1_doubleSpinBox_valueChanged(double arg1)
{
    ui->shapeTextureTrans1_doubleSpinBox->setValue(arg1);
    settings.hiResNoise.translate[0] = arg1;
    realtime->settingsChanged();
}


void MainWindow::on_shapeTextureTrans2_doubleSpinBox_valueChanged(double arg1)
{
    ui->shapeTextureTrans2_doubleSpinBox->setValue(arg1);
    settings.hiResNoise.translate[1] = arg1;
    realtime->settingsChanged();
}


void MainWindow::on_shapeTextureTrans3_doubleSpinBox_valueChanged(double arg1)
{
    ui->shapeTextureTrans3_doubleSpinBox->setValue(arg1);
    settings.hiResNoise.translate[2] = arg1;
    realtime->settingsChanged();
}


void MainWindow::on_shapeTexture_fineR_spinBox_valueChanged(int arg1)
{
    ui->shapeTexture_fineR_spinBox->setValue(arg1);
    settings.hiResNoise.worleyPointsParams[0].cellsPerAxisFine = arg1;
//    std::cout << "changed" << settings.hiResNoise.worleyPointsParams[0].cellsPerAxisFine << '\n';
    settings.newFineArray = true;
    settings.curSlot = 0;
    settings.curChannel = 0;
    realtime->settingsChanged();
}


void MainWindow::on_shapeTexture_mediumR_spinBox_valueChanged(int arg1)
{
    ui->shapeTexture_mediumR_spinBox->setValue(arg1);
    settings.hiResNoise.worleyPointsParams[0].cellsPerAxisMedium = arg1;
    settings.newMediumArray = true;
    settings.curSlot = 0;
    settings.curChannel = 0;
    realtime->settingsChanged();
}


void MainWindow::on_shapeTexture_coarseR_spinBox_valueChanged(int arg1)
{
    ui->shapeTexture_coarseR_spinBox->setValue(arg1);
    settings.hiResNoise.worleyPointsParams[0].cellsPerAxisCoarse = arg1;
    settings.newCoarseArray = true;
    settings.curSlot = 0;
    settings.curChannel = 0;
    realtime->settingsChanged();
}


void MainWindow::on_shapeTexture_weightR_doubleSpinBox_valueChanged(double arg1)
{
    ui->shapeTexture_weightR_doubleSpinBox->setValue(arg1);
    settings.hiResNoise.channelWeights[0] = arg1;
    realtime->settingsChanged();
}


void MainWindow::on_shapeTexture_fineG_spinBox_valueChanged(int arg1)
{
    ui->shapeTexture_fineG_spinBox->setValue(arg1);
    settings.newFineArray = true;
    settings.hiResNoise.worleyPointsParams[1].cellsPerAxisFine = arg1;
//    std::cout << "changed" << settings.hiResNoise.worleyPointsParams[1].cellsPerAxisFine << '\n';
    settings.curSlot = 0;
    settings.curChannel = 1;
    realtime->settingsChanged();
}


void MainWindow::on_shapeTexture_mediumG_spinBox_valueChanged(int arg1)
{
    ui->shapeTexture_mediumG_spinBox->setValue(arg1);
    settings.newMediumArray = true;
    settings.hiResNoise.worleyPointsParams[1].cellsPerAxisMedium = arg1;
    settings.curSlot = 0;
    settings.curChannel = 1;
    realtime->settingsChanged();
}


void MainWindow::on_shapeTexture_coarseG_spinBox_valueChanged(int arg1)
{
    ui->shapeTexture_coarseG_spinBox->setValue(arg1);
    settings.newCoarseArray = true;
    settings.hiResNoise.worleyPointsParams[1].cellsPerAxisCoarse = arg1;
    settings.curSlot = 0;
    settings.curChannel = 1;
    realtime->settingsChanged();
}


void MainWindow::on_shapeTexture_weightG_doubleSpinBox_valueChanged(double arg1)
{
    ui->shapeTexture_weightG_doubleSpinBox->setValue(arg1);
    settings.hiResNoise.channelWeights[1] = arg1;
    realtime->settingsChanged();
}


void MainWindow::on_shapeTexture_fineB_spinBox_valueChanged(int arg1)
{
    ui->shapeTexture_fineB_spinBox->setValue(arg1);
    settings.newFineArray = true;
    settings.hiResNoise.worleyPointsParams[2].cellsPerAxisFine = arg1;
    settings.curSlot = 0;
    settings.curChannel = 2;
    realtime->settingsChanged();
}


void MainWindow::on_shapeTexture_mediumB_spinBox_valueChanged(int arg1)
{
    ui->shapeTexture_mediumB_spinBox->setValue(arg1);
    settings.newMediumArray = true;
    settings.hiResNoise.worleyPointsParams[2].cellsPerAxisMedium = arg1;
    settings.curSlot = 0;
    settings.curChannel = 2;
    realtime->settingsChanged();
}


void MainWindow::on_shapeTexture_coarseB_spinBox_valueChanged(int arg1)
{
    ui->shapeTexture_coarseB_spinBox->setValue(arg1);
    settings.newCoarseArray = true;
    settings.hiResNoise.worleyPointsParams[2].cellsPerAxisCoarse = arg1;
    settings.curSlot = 0;
    settings.curChannel = 2;
    realtime->settingsChanged();
}


void MainWindow::on_shapeTexture_weightB_doubleSpinBox_valueChanged(double arg1)
{
    ui->shapeTexture_weightB_doubleSpinBox->setValue(arg1);
    settings.hiResNoise.channelWeights[2] = arg1;
    realtime->settingsChanged();
}


void MainWindow::on_shapeTexture_fineA_spinBox_valueChanged(int arg1)
{
    ui->shapeTexture_fineA_spinBox->setValue(arg1);
    settings.newFineArray = true;
    settings.hiResNoise.worleyPointsParams[3].cellsPerAxisFine = arg1;
    settings.curSlot = 0;
    settings.curChannel = 3;
    realtime->settingsChanged();
}


void MainWindow::on_shapeTexture_mediumA_spinBox_valueChanged(int arg1)
{
    ui->shapeTexture_mediumA_spinBox->setValue(arg1);
    settings.newMediumArray = true;
    settings.hiResNoise.worleyPointsParams[3].cellsPerAxisMedium = arg1;
    settings.curSlot = 0;
    settings.curChannel = 3;
    realtime->settingsChanged();
}


void MainWindow::on_shapeTexture_coarseA_spinBox_valueChanged(int arg1)
{
    ui->shapeTexture_coarseA_spinBox->setValue(arg1);
    settings.newCoarseArray = true;
    settings.hiResNoise.worleyPointsParams[3].cellsPerAxisCoarse = arg1;
    settings.curSlot = 0;
    settings.curChannel = 3;
    realtime->settingsChanged();
}


void MainWindow::on_shapeTexture_weightA_doubleSpinBox_valueChanged(double arg1)
{
    ui->shapeTexture_weightA_doubleSpinBox->setValue(arg1);
    settings.hiResNoise.channelWeights[3] = arg1;
    realtime->settingsChanged();
}

void MainWindow::on_lightColor_R_doubleSpinBox_valueChanged(double arg1)
{
    ui->lightColor_R_doubleSpinBox->setValue(arg1);
    settings.lightData.color[0] = arg1;
    realtime->settingsChanged();
}


void MainWindow::on_lightColor_G_doubleSpinBox_valueChanged(double arg1)
{
    ui->lightColor_R_doubleSpinBox->setValue(arg1);
    settings.lightData.color[1] = arg1;
    realtime->settingsChanged();
}


void MainWindow::on_lightColor_B_doubleSpinBox_valueChanged(double arg1)
{
    ui->lightColor_R_doubleSpinBox->setValue(arg1);
    settings.lightData.color[2] = arg1;
    realtime->settingsChanged();
}


void MainWindow::on_densityWeight_doubleSpinBox_valueChanged(double arg1)
{
    ui->densityWeight_doubleSpinBox->setValue(arg1);
    settings.loResNoise.densityWeight = arg1;
    realtime->settingsChanged();
}


void MainWindow::on_densityOffset_doubleSpinBox_valueChanged(double arg1)
{
    ui->densityOffset_doubleSpinBox->setValue(arg1);
    settings.hiResNoise.densityOffset = arg1;
    realtime->settingsChanged();
}


void MainWindow::on_highResScaling_R_doubleSpinBox_valueChanged(double arg1)
{
    ui->highResScaling_R_doubleSpinBox->setValue(arg1);
    settings.hiResNoise.scaling[0] = arg1;
    realtime->settingsChanged();
}


void MainWindow::on_highResScaling_G_doubleSpinBox_valueChanged(double arg1)
{
    ui->highResScaling_G_doubleSpinBox->setValue(arg1);
    settings.hiResNoise.scaling[1] = arg1;
    realtime->settingsChanged();
}


void MainWindow::on_highResScaling_B_doubleSpinBox_valueChanged(double arg1)
{
    ui->highResScaling_B_doubleSpinBox->setValue(arg1);
    settings.hiResNoise.scaling[2] = arg1;
    realtime->settingsChanged();
}


void MainWindow::on_highResScaling_A_doubleSpinBox_valueChanged(double arg1)
{
    ui->highResScaling_A_doubleSpinBox->setValue(arg1);
    settings.hiResNoise.scaling[3] = arg1;
    realtime->settingsChanged();
}


void MainWindow::on_lowResScaling_doubleSpinBox_valueChanged(double arg1)
{
    ui->lowResScaling_doubleSpinBox->setValue(arg1);
    settings.loResNoise.scaling[0] = arg1;
    realtime->settingsChanged();
}


void MainWindow::on_volTrans_x_doubleSpinBox_valueChanged(double arg1)
{
    ui->volTrans_x_doubleSpinBox->setValue(arg1);
    settings.volumeTranslate[0] = arg1;
    realtime->settingsChanged();
}


void MainWindow::on_volTrans_y_doubleSpinBox_valueChanged(double arg1)
{
    ui->volTrans_y_doubleSpinBox->setValue(arg1);
    settings.volumeTranslate[1] = arg1;
    realtime->settingsChanged();
}


void MainWindow::on_volTrans_z_doubleSpinBox_valueChanged(double arg1)
{
    ui->volTrans_z_doubleSpinBox->setValue(arg1);
    settings.volumeTranslate[2] = arg1;
    realtime->settingsChanged();
}

