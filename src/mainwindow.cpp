//#include "mainwindow.h"
//#include "settings.h"

//#include <QHBoxLayout>
//#include <QVBoxLayout>
//#include <QFileDialog>
//#include <QSettings>
//#include <QLabel>
//#include <QGroupBox>
//#include <iostream>

//void MainWindow::initialize() {
//    realtime = new Realtime;

//    QHBoxLayout *hLayout = new QHBoxLayout; // horizontal alignment
//    QVBoxLayout *vLayout = new QVBoxLayout(); // vertical alignment
//    vLayout->setAlignment(Qt::AlignTop);
//    hLayout->addLayout(vLayout);
//    hLayout->addWidget(realtime, 1);
//    this->setLayout(hLayout);

//    // Create labels in sidebox
//    QFont font;
//    font.setPointSize(8);
//    font.setBold(true);
//    QLabel *param_label = new QLabel();                 // Parameters label
//    param_label->setText("Volume Rendering Params");
//    param_label->setFont(font);
//    QLabel *param1_label = new QLabel();                // Density multiplier label
//    param1_label->setText("Density Multiplier");
//    QLabel *step_label = new QLabel();                  // Step size label
//    step_label->setText("Step Size");
//    QLabel *cellsFine_label = new QLabel();             // #Cells fine label
//    cellsFine_label->setText("#Cells Fine");
//    QLabel *cellsCoarse_label = new QLabel();           // #Cells coarse label
//    cellsCoarse_label->setText("#Cells Coarse");
//    QLabel *trans_label = new QLabel();                 // volume translate label
//    trans_label->setText("Volume Translation");
//    trans_label->setFont(font);
//    QLabel *scale_label = new QLabel();                 // volume scaling label
//    scale_label->setText("Volume Scaling");
//    scale_label->setFont(font);
//    QLabel *noiseTrans_label = new QLabel();            // noise translate label
//    noiseTrans_label->setText("Noise Translation");
//    noiseTrans_label->setFont(font);
//    QLabel *noiseScale_label = new QLabel();            // noise scaling label
//    noiseScale_label->setText("Noise Scaling");
//    noiseScale_label->setFont(font);
//    QLabel *options_label = new QLabel();               // Options label
//    options_label->setText("Options");
//    options_label->setFont(font);
//    QLabel *ec_label = new QLabel();                    // Extra Credit label
//    ec_label->setText("Load Volume");
//    ec_label->setFont(font);

//    // Create checkbox for density inversion
//    invertDensity = new QCheckBox();
//    invertDensity->setText(QStringLiteral("Invert"));
//    invertDensity->setChecked(true);

//    // Create checkbox for gamma correction
//    gammaCorrect = new QCheckBox();
//    gammaCorrect->setText(QStringLiteral("Gamma Correction"));
//    gammaCorrect->setChecked(false);

//    // Create file uploader for scene file, not used
//    uploadFile = new QPushButton();
//    uploadFile->setText(QStringLiteral("Load Volume File"));

//    // Creates the boxes containing the parameter sliders and number boxes
//    QGroupBox *p1Layout = new QGroupBox(); // horizonal slider 1 alignment
//    QHBoxLayout *l1 = new QHBoxLayout();
//    // Create slider controls to control parameters
//    p1Slider = new QSlider(Qt::Orientation::Horizontal); // Parameter 1 slider
//    p1Slider->setMinimum(0);
//    p1Slider->setMaximum(100);
//    p1Slider->setValue(10);
//    p1Box = new QDoubleSpinBox();
//    p1Box->setMinimum(0.f);
//    p1Box->setMaximum(10.f);
//    p1Box->setValue(1.f);
//    p1Box->setSingleStep(.01f);
//    // Adds the slider and number box to the parameter layouts
//    l1->addWidget(p1Slider);
//    l1->addWidget(p1Box);
//    p1Layout->setLayout(l1);


//    // raymarch step size slider
//    QGroupBox *stepLayout = new QGroupBox(); // horizonal slider alignment
//    QHBoxLayout *lstep = new QHBoxLayout();
//    stepSlider = new QSlider(Qt::Orientation::Horizontal); // Far plane slider
//    stepSlider->setMinimum(1);  // 0.001
//    stepSlider->setMaximum(1000); // 1.0
//    stepSlider->setValue(100);
//    stepBox = new QDoubleSpinBox();
//    stepBox->setMinimum(.1f);
//    stepBox->setMaximum(100.f);
//    stepBox->setSingleStep(.01f);
//    stepBox->setValue(10.f);
//    lstep->addWidget(stepSlider);
//    lstep->addWidget(stepBox);
//    stepLayout->setLayout(lstep);

//    // #cells slider (fine)
//    QGroupBox *cellsFineLayout = new QGroupBox(); // horizonal slider alignment
//    QHBoxLayout *lcellsFine = new QHBoxLayout();
//    cellsFineSlider = new QSlider(Qt::Orientation::Horizontal); // Far plane slider
//    cellsFineSlider->setMinimum(17);
//    cellsFineSlider->setMaximum(32);
//    cellsFineSlider->setValue(32);
//    cellsFineBox = new QSpinBox();
//    cellsFineBox->setMinimum(17);
//    cellsFineBox->setMaximum(32);
//    cellsFineBox->setValue(32);
//    lcellsFine->addWidget(cellsFineSlider);
//    lcellsFine->addWidget(cellsFineBox);
//    cellsFineLayout->setLayout(lcellsFine);

//    // TODO: medium

//    // #cells slider (coarse)
//    QGroupBox *cellsCoarseLayout = new QGroupBox(); // horizonal slider alignment
//    QHBoxLayout *lcellsCoarse = new QHBoxLayout();
//    cellsCoarseSlider = new QSlider(Qt::Orientation::Horizontal); // Far plane slider
//    cellsCoarseSlider->setMinimum(1);
//    cellsCoarseSlider->setMaximum(8);
//    cellsCoarseSlider->setValue(8);
//    cellsCoarseBox = new QSpinBox();
//    cellsCoarseBox->setMinimum(1);
//    cellsCoarseBox->setMaximum(8);
//    cellsCoarseBox->setValue(8);
//    lcellsCoarse->addWidget(cellsCoarseSlider);
//    lcellsCoarse->addWidget(cellsCoarseBox);
//    cellsCoarseLayout->setLayout(lcellsCoarse);

//    // volume translation slider
//    QGroupBox *transLayout = new QGroupBox(); // horizonal slider alignment
//    QHBoxLayout *ltrans = new QHBoxLayout();
//    transxSlider = new QSlider(Qt::Orientation::Horizontal); // x translate slider
//    transxSlider->setMinimum(-20);    // -2.0
//    transxSlider->setMaximum(20);  // 2.0
//    transxSlider->setValue(0);
//    transxBox = new QDoubleSpinBox();
//    transxBox->setMinimum(-2.f);
//    transxBox->setMaximum(+2.f);
//    transxBox->setSingleStep(0.01f);
//    transxBox->setValue(0.f);
//    transySlider = new QSlider(Qt::Orientation::Horizontal); // y translate slider
//    transySlider->setMinimum(-20);    // 0.1
//    transySlider->setMaximum(20);  // 10.0
//    transySlider->setValue(0);
//    transyBox = new QDoubleSpinBox();
//    transyBox->setMinimum(-2.f);
//    transyBox->setMaximum(+2.f);
//    transyBox->setSingleStep(0.01f);
//    transyBox->setValue(0.f);
//    transzSlider = new QSlider(Qt::Orientation::Horizontal); // z translate slider
//    transzSlider->setMinimum(-20);    // 0.1
//    transzSlider->setMaximum(20);  // 10.0
//    transzSlider->setValue(0);
//    transzBox = new QDoubleSpinBox();
//    transzBox->setMinimum(-2.f);
//    transzBox->setMaximum(+2.f);
//    transzBox->setSingleStep(0.01f);
//    transzBox->setValue(0.f);
//    ltrans->addWidget(transxSlider);
//    ltrans->addWidget(transxBox);
//    ltrans->addWidget(transySlider);
//    ltrans->addWidget(transyBox);
//    ltrans->addWidget(transzSlider);
//    ltrans->addWidget(transzBox);
//    transLayout->setLayout(ltrans);


//    // volume scaling slider
//    QGroupBox *scaleLayout = new QGroupBox(); // horizonal slider alignment
//    QHBoxLayout *lscale = new QHBoxLayout();
//    scalexSlider = new QSlider(Qt::Orientation::Horizontal); // x scaling slider
//    scalexSlider->setMinimum(1);
//    scalexSlider->setMaximum(100);
//    scalexSlider->setValue(10);
//    scalexBox = new QDoubleSpinBox();
//    scalexBox->setMinimum(.1f);
//    scalexBox->setMaximum(10.f);
//    scalexBox->setSingleStep(0.1f);
//    scalexBox->setValue(1.f);
//    scaleySlider = new QSlider(Qt::Orientation::Horizontal); // y scaling slider
//    scaleySlider->setMinimum(1);    // 0.1
//    scaleySlider->setMaximum(100);  // 1000.0
//    scaleySlider->setValue(10);
//    scaleyBox = new QDoubleSpinBox();
//    scaleyBox->setMinimum(.1f);
//    scaleyBox->setMaximum(10.f);
//    scaleyBox->setSingleStep(0.1f);
//    scaleyBox->setValue(1.f);
//    scalezSlider = new QSlider(Qt::Orientation::Horizontal); // z scaling slider
//    scalezSlider->setMinimum(1);    // 0.1
//    scalezSlider->setMaximum(100);  // 1000.0
//    scalezSlider->setValue(10);
//    scalezBox = new QDoubleSpinBox();
//    scalezBox->setMinimum(.1f);
//    scalezBox->setMaximum(10.f);
//    scalezBox->setSingleStep(0.1f);
//    scalezBox->setValue(1.f);
//    lscale->addWidget(scalexSlider);
//    lscale->addWidget(scalexBox);
//    lscale->addWidget(scaleySlider);
//    lscale->addWidget(scaleyBox);
//    lscale->addWidget(scalezSlider);
//    lscale->addWidget(scalezBox);
//    scaleLayout->setLayout(lscale);


//    // noise translation slider
//    QGroupBox *noiseTransLayout = new QGroupBox(); // horizonal slider alignment
//    QHBoxLayout *lnoiseTrans = new QHBoxLayout();
//    noiseTransxSlider = new QSlider(Qt::Orientation::Horizontal); // x noiseTranslate slider
//    noiseTransxSlider->setMinimum(-20);    // -2.0
//    noiseTransxSlider->setMaximum(20);  // 2.0
//    noiseTransxSlider->setValue(0);
//    noiseTransxBox = new QDoubleSpinBox();
//    noiseTransxBox->setMinimum(-2.f);
//    noiseTransxBox->setMaximum(+2.f);
//    noiseTransxBox->setSingleStep(0.01f);
//    noiseTransxBox->setValue(0.f);
//    noiseTransySlider = new QSlider(Qt::Orientation::Horizontal); // y noiseTranslate slider
//    noiseTransySlider->setMinimum(-20);    // 0.1
//    noiseTransySlider->setMaximum(20);  // 10.0
//    noiseTransySlider->setValue(0);
//    noiseTransyBox = new QDoubleSpinBox();
//    noiseTransyBox->setMinimum(-2.f);
//    noiseTransyBox->setMaximum(+2.f);
//    noiseTransyBox->setSingleStep(0.01f);
//    noiseTransyBox->setValue(0.f);
//    noiseTranszSlider = new QSlider(Qt::Orientation::Horizontal); // z noiseTranslate slider
//    noiseTranszSlider->setMinimum(-20);    // 0.1
//    noiseTranszSlider->setMaximum(20);  // 10.0
//    noiseTranszSlider->setValue(0);
//    noiseTranszBox = new QDoubleSpinBox();
//    noiseTranszBox->setMinimum(-2.f);
//    noiseTranszBox->setMaximum(+2.f);
//    noiseTranszBox->setSingleStep(0.01f);
//    noiseTranszBox->setValue(0.f);
//    lnoiseTrans->addWidget(noiseTransxSlider);
//    lnoiseTrans->addWidget(noiseTransxBox);
//    lnoiseTrans->addWidget(noiseTransySlider);
//    lnoiseTrans->addWidget(noiseTransyBox);
//    lnoiseTrans->addWidget(noiseTranszSlider);
//    lnoiseTrans->addWidget(noiseTranszBox);
//    noiseTransLayout->setLayout(lnoiseTrans);

//    // noise scaling slider
//    QGroupBox *noiseScaleLayout = new QGroupBox(); // horizonal slider alignment
//    QHBoxLayout *lnoiseScale = new QHBoxLayout();
//    noiseScaleSlider = new QSlider(Qt::Orientation::Horizontal); // Far plane slider
//    noiseScaleSlider->setMinimum(1);
//    noiseScaleSlider->setMaximum(100);
//    noiseScaleSlider->setValue(10);
//    noiseScaleBox = new QDoubleSpinBox();
//    noiseScaleBox->setMinimum(.01f);
//    noiseScaleBox->setMaximum(10.f);
//    noiseScaleBox->setSingleStep(.01f);
//    noiseScaleBox->setValue(1.f);
//    lnoiseScale->addWidget(noiseScaleSlider);
//    lnoiseScale->addWidget(noiseScaleBox);
//    noiseScaleLayout->setLayout(lnoiseScale);


//    // Extra Credit:
//    ec1 = new QCheckBox();
//    ec1->setText(QStringLiteral("Extra Credit 1"));
//    ec1->setChecked(false);

//    ec2 = new QCheckBox();
//    ec2->setText(QStringLiteral("Extra Credit 2"));
//    ec2->setChecked(false);

//    ec3 = new QCheckBox();
//    ec3->setText(QStringLiteral("Load Volume: Foot"));
//    ec3->setChecked(false);

//    ec4 = new QCheckBox();
//    ec4->setText(QStringLiteral("Load Volume: Skull"));
//    ec4->setChecked(false);


//    vLayout->addWidget(uploadFile);
//    vLayout->addWidget(param_label);
//    vLayout->addWidget(param1_label);
//    vLayout->addWidget(p1Layout);
//    vLayout->addWidget(step_label);
//    vLayout->addWidget(stepLayout);

//    vLayout->addWidget(cellsFine_label);
//    vLayout->addWidget(cellsFineLayout);
//    vLayout->addWidget(cellsCoarse_label);
//    vLayout->addWidget(cellsCoarseLayout);

//    vLayout->addWidget(trans_label);
//    vLayout->addWidget(transLayout);
//    vLayout->addWidget(scale_label);
//    vLayout->addWidget(scaleLayout);

//    vLayout->addWidget(noiseTrans_label);
//    vLayout->addWidget(noiseTransLayout);
//    vLayout->addWidget(noiseScale_label);
//    vLayout->addWidget(noiseScaleLayout);

//    vLayout->addWidget(options_label);
//    vLayout->addWidget(invertDensity);
//    vLayout->addWidget(gammaCorrect);

////    vLayout->addWidget(ec_label);
////    vLayout->addWidget(ec3);
////    vLayout->addWidget(ec4);

//    connectUIElements();
//}

//void MainWindow::finish() {
//    realtime->finish();
//    delete(realtime);
//}

//void MainWindow::connectUIElements() {
//    connectInvertDensity();
//    connectGammaCorrect();
//    connectUploadFile();
//    connectParam1();
//    connectTrans();
//    connectScaling();
//    connectNoiseTrans();
//    connectNoiseScaling();
//    connectStep();
//    connectNumCells();
//    connectExtraCredit();
//}

//void MainWindow::connectInvertDensity() {
//    connect(invertDensity, &QCheckBox::clicked, this, &MainWindow::onInvertDensity);
//}

//void MainWindow::connectGammaCorrect() {
//    connect(gammaCorrect, &QCheckBox::clicked, this, &MainWindow::onGammaCorrect);
//}

//void MainWindow::connectUploadFile() {
//    connect(uploadFile, &QPushButton::clicked, this, &MainWindow::onUploadFile);
//}

//void MainWindow::connectParam1() {
//    connect(p1Slider, &QSlider::valueChanged, this, &MainWindow::onValChangeP1Slider);
//    connect(p1Box, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
//            this, &MainWindow::onValChangeP1Box);
//}

//void MainWindow::connectNumCells() {
//    connect(cellsFineSlider, &QSlider::valueChanged, this, &MainWindow::onValChangeCellsFine);
//    connect(cellsFineBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
//            this, &MainWindow::onValChangeCellsFine);
//    connect(cellsCoarseSlider, &QSlider::valueChanged, this, &MainWindow::onValChangeCellsCoarse);
//    connect(cellsCoarseBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
//            this, &MainWindow::onValChangeCellsCoarse);
//}

//void MainWindow::connectTrans() {
//    connect(transxSlider, &QSlider::valueChanged, this, &MainWindow::onValChangeTransxSlider);
//    connect(transxBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
//            this, &MainWindow::onValChangeTransxBox);
//    connect(transySlider, &QSlider::valueChanged, this, &MainWindow::onValChangeTransySlider);
//    connect(transyBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
//            this, &MainWindow::onValChangeTransyBox);
//    connect(transzSlider, &QSlider::valueChanged, this, &MainWindow::onValChangeTranszSlider);
//    connect(transzBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
//            this, &MainWindow::onValChangeTranszBox);
//}

//void MainWindow::connectScaling() {
//    connect(scalexSlider, &QSlider::valueChanged, this, &MainWindow::onValChangeScalexSlider);
//    connect(scalexBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
//            this, &MainWindow::onValChangeScalexBox);
//    connect(scaleySlider, &QSlider::valueChanged, this, &MainWindow::onValChangeScaleySlider);
//    connect(scaleyBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
//            this, &MainWindow::onValChangeScaleyBox);
//    connect(scalezSlider, &QSlider::valueChanged, this, &MainWindow::onValChangeScalezSlider);
//    connect(scalezBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
//            this, &MainWindow::onValChangeScalezBox);
//}

//void MainWindow::connectNoiseScaling() {
//    connect(noiseScaleSlider, &QSlider::valueChanged, this, &MainWindow::onValChangeNoiseScaleSlider);
//    connect(noiseScaleBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
//            this, &MainWindow::onValChangeNoiseScaleBox);
//}

//void MainWindow::connectNoiseTrans() {
//    connect(noiseTransxSlider, &QSlider::valueChanged, this, &MainWindow::onValChangeNoiseTransxSlider);
//    connect(noiseTransxBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
//            this, &MainWindow::onValChangeNoiseTransxBox);
//    connect(noiseTransySlider, &QSlider::valueChanged, this, &MainWindow::onValChangeNoiseTransySlider);
//    connect(noiseTransyBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
//            this, &MainWindow::onValChangeNoiseTransyBox);
//    connect(noiseTranszSlider, &QSlider::valueChanged, this, &MainWindow::onValChangeNoiseTranszSlider);
//    connect(noiseTranszBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
//            this, &MainWindow::onValChangeNoiseTranszBox);
//}


//void MainWindow::connectStep() {
//    connect(stepSlider, &QSlider::valueChanged, this, &MainWindow::onValChangeStepSlider);
//    connect(stepBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
//            this, &MainWindow::onValChangeStepBox);
//}

//void MainWindow::connectExtraCredit() {
//    connect(ec1, &QCheckBox::clicked, this, &MainWindow::onExtraCredit1);
//    connect(ec2, &QCheckBox::clicked, this, &MainWindow::onExtraCredit2);
//    connect(ec3, &QCheckBox::clicked, this, &MainWindow::onExtraCredit3);
//    connect(ec4, &QCheckBox::clicked, this, &MainWindow::onExtraCredit4);
//}

//void MainWindow::onInvertDensity() {
//    settings.invertDensity = !settings.invertDensity;
//    realtime->settingsChanged();
//}

//void MainWindow::onGammaCorrect() {
//    settings.gammaCorrect = !settings.gammaCorrect;
//    realtime->settingsChanged();
//}

//void MainWindow::onUploadFile() {
//    // Get abs path of scene file
//    QString volumeFilePath = QFileDialog::getOpenFileName(this, tr("Upload File"), QDir::homePath(), tr("Volume Files (*.raw)"));
//    if (volumeFilePath.isNull()) {
//        std::cout << "No volume files selected." << std::endl;
//        return;
//    }

//    settings.volumeFilePath = volumeFilePath.toStdString();

//    std::cout << "Loaded volume: \"" << volumeFilePath.toStdString() << "\"." << std::endl;

//    realtime->volumeChanged();
//}


//// on change: volume density multiplier
//void MainWindow::onValChangeP1Box(double newValue) {
//    p1Slider->setValue(int(newValue*10.f));
//    p1Box->setValue(newValue);
//    settings.densityMult = p1Box->value();
//    realtime->settingsChanged();
//}
//void MainWindow::onValChangeP1Slider(int newValue) {
//    p1Slider->setValue(newValue);
//    p1Box->setValue(newValue/10.f);
//    settings.densityMult = p1Box->value();
//    realtime->settingsChanged();
//}

//// on change: ray marching step size
//void MainWindow::onValChangeStepSlider(int newValue) {
//    stepSlider->setValue(newValue);
//    stepBox->setValue(newValue/10.f);
//    settings.stepSize = stepBox->value();
//    realtime->settingsChanged();
//}
//void MainWindow::onValChangeStepBox(double newValue) {
//    stepSlider->setValue(int(newValue*10.f));
//    stepBox->setValue(newValue);
//    settings.stepSize = stepBox->value();
//    realtime->settingsChanged();
//}

//// on change: #cells
//void MainWindow::onValChangeCellsFine(int newValue) {
//    cellsFineSlider->setValue(newValue);
//    cellsFineBox->setValue(newValue);
//    settings.hiResNoise.worleyPointsParams->cellsPerAxisFine = cellsFineBox->value();
//    settings.newFineArray = true;  // tell settingsChanged to update fine array
//    realtime->settingsChanged();
//}
////void MainWindow::onValChangeCellsMedium(int newValue) {
////    cellsMediumSlider->setValue(newValue);
////    cellsMediumBox->setValue(newValue);
////    settings.cellsPerAxisFine = cellsMediumBox->value();
////    settings.newMediumArray = true;  // tell settingsChanged to update medium array
////    realtime->settingsChanged();
////}
//void MainWindow::onValChangeCellsCoarse(int newValue) {
//    cellsCoarseSlider->setValue(newValue);
//    cellsCoarseBox->setValue(newValue);
//    settings.hiResNoise.worleyPointsParams->cellsPerAxisCoarse = cellsCoarseBox->value();
//    settings.newCoarseArray = true;  // tell settingsChanged to update coarse array
//    realtime->settingsChanged();
//}


//// on change: volume translation
//void MainWindow::onValChangeTransxBox(double newValue) {
//    transxSlider->setValue(int(newValue*10.f));
//    transxBox->setValue(newValue);
//    settings.volumeTranslate.x = transxBox->value();
//    realtime->settingsChanged();
//}

//void MainWindow::onValChangeTransxSlider(int newValue) {
//    transxSlider->setValue(newValue);
//    transxBox->setValue(newValue/10.f);
//    settings.volumeTranslate.x = transxBox->value();
//    realtime->settingsChanged();
//}

//void MainWindow::onValChangeTransyBox(double newValue) {
//    transySlider->setValue(int(newValue*10.f));
//    transyBox->setValue(newValue);
//    settings.volumeTranslate.y = transyBox->value();
//    realtime->settingsChanged();
//}

//void MainWindow::onValChangeTransySlider(int newValue) {
//    transySlider->setValue(newValue);
//    transyBox->setValue(newValue/10.f);
//    settings.volumeTranslate.y = transyBox->value();
//    realtime->settingsChanged();
//}

//void MainWindow::onValChangeTranszBox(double newValue) {
//    transzSlider->setValue(int(newValue*10.f));
//    transzBox->setValue(newValue);
//    settings.volumeTranslate.z = transzBox->value();
//    realtime->settingsChanged();
//}

//void MainWindow::onValChangeTranszSlider(int newValue) {
//    transzSlider->setValue(newValue);
//    transzBox->setValue(newValue/10.f);
//    settings.volumeTranslate.z = transzBox->value();
//    realtime->settingsChanged();
//}

//// on change: volume scaling
//void MainWindow::onValChangeScalexBox(double newValue) {
//    scalexSlider->setValue(int(newValue*10.f));
//    scalexBox->setValue(newValue);
//    settings.volumeScaling.x = scalexBox->value();
//    realtime->settingsChanged();
//}

//void MainWindow::onValChangeScalexSlider(int newValue) {
//    scalexSlider->setValue(newValue);
//    scalexBox->setValue(newValue/10.f);
//    settings.volumeScaling.x = scalexBox->value();
//    realtime->settingsChanged();
//}

//void MainWindow::onValChangeScaleyBox(double newValue) {
//    scaleySlider->setValue(int(newValue*10.f));
//    scaleyBox->setValue(newValue);
//    settings.volumeScaling.y = scaleyBox->value();
//    realtime->settingsChanged();
//}

//void MainWindow::onValChangeScaleySlider(int newValue) {
//    scaleySlider->setValue(newValue);
//    scaleyBox->setValue(newValue/10.f);
//    settings.volumeScaling.y = scaleyBox->value();
//    realtime->settingsChanged();
//}

//void MainWindow::onValChangeScalezBox(double newValue) {
//    scalezSlider->setValue(int(newValue*10.f));
//    scalezBox->setValue(newValue);
//    settings.volumeScaling.z = scalezBox->value();
//    realtime->settingsChanged();
//}

//void MainWindow::onValChangeScalezSlider(int newValue) {
//    scalezSlider->setValue(newValue);
//    scalezBox->setValue(newValue/10.f);
//    settings.volumeScaling.z = scalezBox->value();
//    realtime->settingsChanged();
//}


//// on change: noise translation
//void MainWindow::onValChangeNoiseTransxBox(double newValue) {
//    noiseTransxSlider->setValue(int(newValue*10.f));
//    noiseTransxBox->setValue(newValue);
//    settings.hiResNoise.translate.x = noiseTransxBox->value();
//    realtime->settingsChanged();
//}

//void MainWindow::onValChangeNoiseTransxSlider(int newValue) {
//    noiseTransxSlider->setValue(newValue);
//    noiseTransxBox->setValue(newValue/10.f);
//    settings.hiResNoise.translate.x = noiseTransxBox->value();
//    realtime->settingsChanged();
//}

//void MainWindow::onValChangeNoiseTransyBox(double newValue) {
//    noiseTransySlider->setValue(int(newValue*10.f));
//    noiseTransyBox->setValue(newValue);
//    settings.hiResNoise.translate.y = noiseTransyBox->value();
//    realtime->settingsChanged();
//}

//void MainWindow::onValChangeNoiseTransySlider(int newValue) {
//    noiseTransySlider->setValue(newValue);
//    noiseTransyBox->setValue(newValue/10.f);
//    settings.hiResNoise.translate.y = noiseTransyBox->value();
//    realtime->settingsChanged();
//}

//void MainWindow::onValChangeNoiseTranszBox(double newValue) {
//    noiseTranszSlider->setValue(int(newValue*10.f));
//    noiseTranszBox->setValue(newValue);
//    settings.hiResNoise.translate.z = noiseTranszBox->value();
//    realtime->settingsChanged();
//}

//void MainWindow::onValChangeNoiseTranszSlider(int newValue) {
//    noiseTranszSlider->setValue(newValue);
//    noiseTranszBox->setValue(newValue/10.f);
//    settings.hiResNoise.translate.z = noiseTranszBox->value();
//    realtime->settingsChanged();
//}


//// on change: noise scaling
//void MainWindow::onValChangeNoiseScaleBox(double newValue) {
//    noiseScaleSlider->setValue(int(newValue*10.f));
//    noiseScaleBox->setValue(newValue);
//    settings.hiResNoise.scaling = noiseScaleBox->value();
//    realtime->settingsChanged();
//}
//void MainWindow::onValChangeNoiseScaleSlider(int newValue) {
//    noiseScaleSlider->setValue(newValue);
//    noiseScaleBox->setValue(newValue/10.f);
//    settings.hiResNoise.scaling = noiseScaleBox->value();
//    realtime->settingsChanged();
//}




//// Extra Credit:

//void MainWindow::onExtraCredit1() {
//    settings.extraCredit1 = !settings.extraCredit1;
//    realtime->settingsChanged();
//}

//void MainWindow::onExtraCredit2() {
//    settings.extraCredit2 = !settings.extraCredit2;
//    realtime->settingsChanged();
//}

//void MainWindow::onExtraCredit3() {
//    settings.extraCredit3 = !settings.extraCredit3;
//    realtime->settingsChanged();
//}

//void MainWindow::onExtraCredit4() {
//    settings.extraCredit4 = !settings.extraCredit4;
//    realtime->settingsChanged();
//}


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
    ///TO:DO change appropriate setting in settings.
    realtime->settingsChanged();
}


void MainWindow::on_lightTrans_doubleSpinBox_valueChanged(double arg1)
{
    ui->lightTrans_doubleSpinBox->setValue(arg1);
   ///TO:DO change appropriate setting in settings.
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


void MainWindow::on_noiseScaling_horizontalSlider_sliderMoved(int position)
{
    ui->noiseScaling_horizontalSlider->setValue(position);
    ui->noiseScaling_doubleSpinBox->setValue(position);
    realtime->settingsChanged();

}


void MainWindow::on_noiseScaling_doubleSpinBox_valueChanged(double arg1)
{
    ui->noiseScaling_doubleSpinBox->setValue(arg1);
    ui->noiseScaling_horizontalSlider->setValue(arg1);
    ///TO:DO change appropriate setting in settings.
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


void MainWindow::on_lightColor_R_spinBox_valueChanged(int arg1)
{
    ui->lightColor_R_spinBox->setValue(arg1);
    ///TO:DO change appropriate setting in settings.
    realtime->settingsChanged();

}


void MainWindow::on_lightColor_G_spinBox_valueChanged(int arg1)
{
    ui->lightColor_G_spinBox->setValue(arg1);
    ///TO:DO change appropriate setting in settings.
    realtime->settingsChanged();
}


void MainWindow::on_lightColor_B_spinBox_valueChanged(int arg1)
{
    ui->lightColor_B_spinBox->setValue(arg1);
    ///TO:DO change appropriate setting in settings.
    realtime->settingsChanged();
}


void MainWindow::on_lightColor_A_spinBox_valueChanged(int arg1)
{
    ui->lightColor_A_spinBox->setValue(arg1);
    ///TO:DO change appropriate setting in settings.
    realtime->settingsChanged();
}


void MainWindow::on_lightLatitude_horizontalSlider_sliderMoved(int position)
{
    ui->lightLatitude_horizontalSlider->setValue(position);
    ui->lightLatitude_doubleSpinBox->setValue(position);
    ///TO:DO change appropriate setting in settings.
    realtime->settingsChanged();
}


void MainWindow::on_lightLongitude_horizontalSlider_sliderMoved(int position)
{
    ui->lightLongitude_horizontalSlider->setValue(position);
    ui->lightLongitude_doubleSpinBox->setValue(position);
    ///TO:DO change appropriate setting in settings.
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
    ///TO:DO change appropriate setting in settings.
    realtime->settingsChanged();
}


void MainWindow::on_detailTextureTrans2_doubleSpinBox_valueChanged(double arg1)
{
    ui->detailTextureTrans2_doubleSpinBox->setValue(arg1);
    ///TO:DO change appropriate setting in settings.
    realtime->settingsChanged();
}


void MainWindow::on_detailTextureTrans3_doubleSpinBox_valueChanged(double arg1)
{
    ui->detailTextureTrans3_doubleSpinBox->setValue(arg1);
    ///TO:DO change appropriate setting in settings.
    realtime->settingsChanged();
}


void MainWindow::on_detailTexture_fineR_spinBox_valueChanged(int arg1)
{
    ui->detailTexture_fineR_spinBox->setValue(arg1);
    ///TO:DO change appropriate setting in settings.
    // settings.newFineArray = arg1; ?
    realtime->settingsChanged();
}


void MainWindow::on_detailTexture_mediumR_spinBox_valueChanged(int arg1)
{
    ui->detailTexture_mediumR_spinBox->setValue(arg1);
    ///TO:DO change appropriate setting in settings.
    realtime->settingsChanged();
}


void MainWindow::on_detailTexture_coarseR_spinBox_valueChanged(int arg1)
{
    ui->detailTexture_coarseR_spinBox->setValue(arg1);
    ///TO:DO change appropriate setting in settings.
    realtime->settingsChanged();
}


void MainWindow::on_detailTexture_weightR_spinBox_valueChanged(double arg1)
{
    ui->detailTexture_weightR_spinBox->setValue(arg1);
    ///TO:DO change appropriate setting in settings.
    realtime->settingsChanged();
}


void MainWindow::on_detailTexture_fineG_spinBox_valueChanged(int arg1)
{
    ui->detailTexture_fineG_spinBox->setValue(arg1);
    ///TO:DO change appropriate setting in settings.
    realtime->settingsChanged();
}


void MainWindow::on_detailTexture_mediumG_spinBox_valueChanged(int arg1)
{
    ui->detailTexture_mediumG_spinBox->setValue(arg1);
    ///TO:DO change appropriate setting in settings.
    realtime->settingsChanged();
}


void MainWindow::on_detailTexture_coarseG_spinBox_valueChanged(int arg1)
{
    ui->detailTexture_coarseG_spinBox->setValue(arg1);
    ///TO:DO change appropriate setting in settings.
    realtime->settingsChanged();
}


void MainWindow::on_detailTexture_weightG_spinBox_valueChanged(double arg1)
{
    ui->detailTexture_weightG_spinBox->setValue(arg1);
    ///TO:DO change appropriate setting in settings.
    realtime->settingsChanged();
}


void MainWindow::on_detailTexture_fineB_spinBox_valueChanged(int arg1)
{
    ui->detailTexture_fineB_spinBox->setValue(arg1);
    ///TO:DO change appropriate setting in settings.
    realtime->settingsChanged();
}


void MainWindow::on_detailTexture_mediumB_spinBox_valueChanged(int arg1)
{
    ui->detailTexture_mediumB_spinBox->setValue(arg1);
    ///TO:DO change appropriate setting in settings.
    realtime->settingsChanged();
}


void MainWindow::on_detailTexture_coarseB_spinBox_valueChanged(int arg1)
{
    ui->detailTexture_coarseB_spinBox->setValue(arg1);
    ///TO:DO change appropriate setting in settings.
    realtime->settingsChanged();
}


void MainWindow::on_detailTexture_weightB_spinBox_valueChanged(double arg1)
{
    ui->detailTexture_weightB_spinBox->setValue(arg1);
    ///TO:DO change appropriate setting in settings.
    realtime->settingsChanged();
}


void MainWindow::on_detailTexture_fineA_spinBox_valueChanged(int arg1)
{
    ui->detailTexture_fineA_spinBox->setValue(arg1);
    ///TO:DO change appropriate setting in settings.
    realtime->settingsChanged();
}


void MainWindow::on_detailTexture_mediumA_spinBox_valueChanged(int arg1)
{
    ui->detailTexture_mediumA_spinBox->setValue(arg1);
    ///TO:DO change appropriate setting in settings.
    realtime->settingsChanged();
}


void MainWindow::on_detailTexture_coarseA_spinBox_valueChanged(int arg1)
{
    ui->detailTexture_coarseA_spinBox->setValue(arg1);
    ///TO:DO change appropriate setting in settings.
    realtime->settingsChanged();
}


void MainWindow::on_detailTexture_weightA_spinBox_valueChanged(double arg1)
{
    ui->detailTexture_weightA_spinBox->setValue(arg1);
    ///TO:DO change appropriate setting in settings.
    realtime->settingsChanged();
}


void MainWindow::on_shapeTextureTrans1_doubleSpinBox_valueChanged(double arg1)
{
    ui->shapeTextureTrans1_doubleSpinBox->setValue(arg1);
    ///TO:DO change appropriate setting in settings.
    realtime->settingsChanged();
}


void MainWindow::on_shapeTextureTrans2_doubleSpinBox_valueChanged(double arg1)
{
    ui->shapeTextureTrans2_doubleSpinBox->setValue(arg1);
    ///TO:DO change appropriate setting in settings.
    realtime->settingsChanged();
}


void MainWindow::on_shapeTextureTrans3_doubleSpinBox_valueChanged(double arg1)
{
    ui->shapeTextureTrans3_doubleSpinBox->setValue(arg1);
    ///TO:DO change appropriate setting in settings.
    realtime->settingsChanged();
}


void MainWindow::on_shapeTexture_fineR_spinBox_valueChanged(int arg1)
{
    ui->shapeTexture_fineR_spinBox->setValue(arg1);
    ///TO:DO change appropriate setting in settings.
    realtime->settingsChanged();
}


void MainWindow::on_shapeTexture_mediumR_spinBox_valueChanged(int arg1)
{
    ui->shapeTexture_mediumR_spinBox->setValue(arg1);
    ///TO:DO change appropriate setting in settings.
    realtime->settingsChanged();
}


void MainWindow::on_shapeTexture_coarseR_spinBox_valueChanged(int arg1)
{
    ui->shapeTexture_coarseR_spinBox->setValue(arg1);
    ///TO:DO change appropriate setting in settings.
    realtime->settingsChanged();
}


void MainWindow::on_shapeTexture_weightR_doubleSpinBox_valueChanged(double arg1)
{
    ui->shapeTexture_weightR_doubleSpinBox->setValue(arg1);
    ///TO:DO change appropriate setting in settings.
    realtime->settingsChanged();
}


void MainWindow::on_shapeTexture_fineG_spinBox_valueChanged(int arg1)
{
    ui->shapeTexture_fineG_spinBox->setValue(arg1);
    ///TO:DO change appropriate setting in settings.
    realtime->settingsChanged();
}


void MainWindow::on_shapeTexture_mediumG_spinBox_valueChanged(int arg1)
{
    ui->shapeTexture_mediumG_spinBox->setValue(arg1);
    ///TO:DO change appropriate setting in settings.
    realtime->settingsChanged();
}


void MainWindow::on_shapeTexture_coarseG_spinBox_valueChanged(int arg1)
{
    ui->shapeTexture_coarseG_spinBox->setValue(arg1);
    ///TO:DO change appropriate setting in settings.
    realtime->settingsChanged();
}


void MainWindow::on_shapeTexture_weightG_doubleSpinBox_valueChanged(double arg1)
{
    ui->shapeTexture_weightG_doubleSpinBox->setValue(arg1);
    ///TO:DO change appropriate setting in settings.
    realtime->settingsChanged();
}


void MainWindow::on_shapeTexture_fineB_spinBox_valueChanged(int arg1)
{
    ui->shapeTexture_fineB_spinBox->setValue(arg1);
    ///TO:DO change appropriate setting in settings.
    realtime->settingsChanged();
}


void MainWindow::on_shapeTexture_mediumB_spinBox_valueChanged(int arg1)
{
    ui->shapeTexture_mediumB_spinBox->setValue(arg1);
    ///TO:DO change appropriate setting in settings.
    realtime->settingsChanged();
}


void MainWindow::on_shapeTexture_coarseB_spinBox_valueChanged(int arg1)
{
    ui->shapeTexture_coarseB_spinBox->setValue(arg1);
    ///TO:DO change appropriate setting in settings.
    realtime->settingsChanged();
}


void MainWindow::on_shapeTexture_weightB_doubleSpinBox_valueChanged(double arg1)
{
    ui->shapeTexture_weightB_doubleSpinBox->setValue(arg1);
    ///TO:DO change appropriate setting in settings.
    realtime->settingsChanged();
}


void MainWindow::on_shapeTexture_fineA_spinBox_valueChanged(int arg1)
{
    ui->shapeTexture_fineA_spinBox->setValue(arg1);
    ///TO:DO change appropriate setting in settings.
    realtime->settingsChanged();
}


void MainWindow::on_shapeTexture_mediumA_spinBox_valueChanged(int arg1)
{
    ui->shapeTexture_mediumA_spinBox->setValue(arg1);
    ///TO:DO change appropriate setting in settings.
    realtime->settingsChanged();
}


void MainWindow::on_shapeTexture_coarseA_spinBox_valueChanged(int arg1)
{
    ui->shapeTexture_coarseA_spinBox->setValue(arg1);
    ///TO:DO change appropriate setting in settings.
    realtime->settingsChanged();
}


void MainWindow::on_shapeTexture_weightA_doubleSpinBox_valueChanged(double arg1)
{
    ui->shapeTexture_weightA_doubleSpinBox->setValue(arg1);
    ///TO:DO change appropriate setting in settings.
    realtime->settingsChanged();
}
