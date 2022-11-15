#include "mainwindow.h"
#include "settings.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QSettings>
#include <QLabel>
#include <QGroupBox>
#include <iostream>

void MainWindow::initialize() {
    realtime = new Realtime;

    QHBoxLayout *hLayout = new QHBoxLayout; // horizontal alignment
    QVBoxLayout *vLayout = new QVBoxLayout(); // vertical alignment
    vLayout->setAlignment(Qt::AlignTop);
    hLayout->addLayout(vLayout);
    hLayout->addWidget(realtime, 1);
    this->setLayout(hLayout);

    // Create labels in sidebox
    QFont font;
    font.setPointSize(12);
    font.setBold(true);
    QLabel *param_label = new QLabel(); // Parameters label
    param_label->setText("Volume Rendering Params");
    param_label->setFont(font);
    QLabel *camera_label = new QLabel(); // Camera label
    camera_label->setText("Camera");
    camera_label->setFont(font);
    QLabel *filters_label = new QLabel(); // Filters label
    filters_label->setText("Dummy");
    filters_label->setFont(font);
    QLabel *ec_label = new QLabel(); // Extra Credit label
    ec_label->setText("Load Volume");
    ec_label->setFont(font);
    QLabel *param1_label = new QLabel(); // Parameter 1 label
    param1_label->setText("Interval Length");
    QLabel *near_label = new QLabel(); // Near plane label
    near_label->setText("Near Plane:");
    QLabel *far_label = new QLabel(); // Far plane label
    far_label->setText("Far Plane:");



    // Create checkbox for per-pixel filter
    filter1 = new QCheckBox();
    filter1->setText(QStringLiteral("Dummy Checkbox"));
    filter1->setChecked(false);

    // Create checkbox for kernel-based filter
    filter2 = new QCheckBox();
    filter2->setText(QStringLiteral("Dummy Checkbox"));
    filter2->setChecked(false);

    // Create file uploader for scene file
    uploadFile = new QPushButton();
    uploadFile->setText(QStringLiteral("Load Volume File"));

    // Creates the boxes containing the parameter sliders and number boxes
    QGroupBox *p1Layout = new QGroupBox(); // horizonal slider 1 alignment
    QHBoxLayout *l1 = new QHBoxLayout();

    // Create slider controls to control parameters
    p1Slider = new QSlider(Qt::Orientation::Horizontal); // Parameter 1 slider
    p1Slider->setMinimum(1);
    p1Slider->setMaximum(500);
    p1Slider->setValue(150);

    p1Box = new QDoubleSpinBox();
    p1Box->setMinimum(0.1f);
    p1Box->setMaximum(50.f);
    p1Box->setValue(15.f);

    // Adds the slider and number box to the parameter layouts
    l1->addWidget(p1Slider);
    l1->addWidget(p1Box);
    p1Layout->setLayout(l1);


    // Creates the boxes containing the camera sliders and number boxes
    QGroupBox *nearLayout = new QGroupBox(); // horizonal near slider alignment
    QHBoxLayout *lnear = new QHBoxLayout();
    QGroupBox *farLayout = new QGroupBox(); // horizonal far slider alignment
    QHBoxLayout *lfar = new QHBoxLayout();

    // Create slider controls to control near/far planes
    nearSlider = new QSlider(Qt::Orientation::Horizontal); // Near plane slider
    nearSlider->setMinimum(1);    // 0.1
    nearSlider->setMaximum(100);  // 10.0
    nearSlider->setValue(1);

    nearBox = new QDoubleSpinBox();
    nearBox->setMinimum(0.1f);
    nearBox->setMaximum(10.f);
    nearBox->setSingleStep(0.1f);
    nearBox->setValue(0.1f);


    farSlider = new QSlider(Qt::Orientation::Horizontal); // Far plane slider
    farSlider->setMinimum(100);  // 10.0
    farSlider->setMaximum(100000); // 10000.0
    farSlider->setValue(100000);

    farBox = new QDoubleSpinBox();
    farBox->setMinimum(10.f);
    farBox->setMaximum(10000.f);
    farBox->setSingleStep(1.f);
    farBox->setValue(10000.f);

    // Adds the slider and number box to the parameter layouts
    lnear->addWidget(nearSlider);
    lnear->addWidget(nearBox);
    nearLayout->setLayout(lnear);

    lfar->addWidget(farSlider);
    lfar->addWidget(farBox);
    farLayout->setLayout(lfar);

    // Extra Credit:
    ec1 = new QCheckBox();
    ec1->setText(QStringLiteral("Extra Credit 1"));
    ec1->setChecked(false);

    ec2 = new QCheckBox();
    ec2->setText(QStringLiteral("Extra Credit 2"));
    ec2->setChecked(false);

    ec3 = new QCheckBox();
    ec3->setText(QStringLiteral("Load Volume: Foot"));
    ec3->setChecked(false);

    ec4 = new QCheckBox();
    ec4->setText(QStringLiteral("Load Volume: Skull"));
    ec4->setChecked(false);

    vLayout->addWidget(uploadFile);
    vLayout->addWidget(param_label);
    vLayout->addWidget(param1_label);
    vLayout->addWidget(p1Layout);
    vLayout->addWidget(camera_label);
    vLayout->addWidget(near_label);
    vLayout->addWidget(nearLayout);
    vLayout->addWidget(far_label);
    vLayout->addWidget(farLayout);
    vLayout->addWidget(filters_label);
    vLayout->addWidget(filter1);
    vLayout->addWidget(filter2);
    // Extra Credit:
    vLayout->addWidget(ec_label);
//    vLayout->addWidget(ec1);
//    vLayout->addWidget(ec2);
    vLayout->addWidget(ec3);
    vLayout->addWidget(ec4);

    connectUIElements();

    // Set default values of volume rendering params
    onValChangeP1Box(15.f);

    // Set default values for near and far planes
    onValChangeNearBox(0.1f);
    onValChangeFarBox(10000.f);
}

void MainWindow::finish() {
    realtime->finish();
    delete(realtime);
}

void MainWindow::connectUIElements() {
    connectPerPixelFilter();
    connectKernelBasedFilter();
    connectUploadFile();
    connectParam1();
    connectNear();
    connectFar();
    connectExtraCredit();
}

void MainWindow::connectPerPixelFilter() {
    connect(filter1, &QCheckBox::clicked, this, &MainWindow::onPerPixelFilter);
}

void MainWindow::connectKernelBasedFilter() {
    connect(filter2, &QCheckBox::clicked, this, &MainWindow::onKernelBasedFilter);
}

void MainWindow::connectUploadFile() {
    connect(uploadFile, &QPushButton::clicked, this, &MainWindow::onUploadFile);
}

void MainWindow::connectParam1() {
    connect(p1Slider, &QSlider::valueChanged, this, &MainWindow::onValChangeP1Slider);
    connect(p1Box, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, &MainWindow::onValChangeP1Box);
}

void MainWindow::connectNear() {
    connect(nearSlider, &QSlider::valueChanged, this, &MainWindow::onValChangeNearSlider);
    connect(nearBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, &MainWindow::onValChangeNearBox);
}

void MainWindow::connectFar() {
    connect(farSlider, &QSlider::valueChanged, this, &MainWindow::onValChangeFarSlider);
    connect(farBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, &MainWindow::onValChangeFarBox);
}

void MainWindow::connectExtraCredit() {
    connect(ec1, &QCheckBox::clicked, this, &MainWindow::onExtraCredit1);
    connect(ec2, &QCheckBox::clicked, this, &MainWindow::onExtraCredit2);
    connect(ec3, &QCheckBox::clicked, this, &MainWindow::onExtraCredit3);
    connect(ec4, &QCheckBox::clicked, this, &MainWindow::onExtraCredit4);
}

void MainWindow::onPerPixelFilter() {
    settings.perPixelFilter = !settings.perPixelFilter;
    realtime->settingsChanged();
}

void MainWindow::onKernelBasedFilter() {
    settings.kernelBasedFilter = !settings.kernelBasedFilter;
    realtime->settingsChanged();
}

void MainWindow::onUploadFile() {
    // Get abs path of scene file
    QString volumeFilePath = QFileDialog::getOpenFileName(this, tr("Upload File"), QDir::homePath(), tr("Volume Files (*.raw)"));
    if (volumeFilePath.isNull()) {
        std::cout << "No volume files selected." << std::endl;
        return;
    }

    settings.volumeFilePath = volumeFilePath.toStdString();

    std::cout << "Loaded volume: \"" << volumeFilePath.toStdString() << "\"." << std::endl;

    realtime->volumeChanged();
}


void MainWindow::onValChangeP1Box(double newValue) {
    p1Slider->setValue(int(newValue*10.f));
    p1Box->setValue(newValue);
    settings.intervalLength = p1Box->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeP1Slider(int newValue) {
    p1Slider->setValue(newValue);
    p1Box->setValue(newValue/10.f);
    settings.intervalLength = p1Box->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeNearSlider(int newValue) {
    nearSlider->setValue(newValue);
    nearBox->setValue(newValue/10.f);
    settings.nearPlane = nearBox->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeFarSlider(int newValue) {
    farSlider->setValue(newValue);
    farBox->setValue(newValue/10.f);
    settings.farPlane = farBox->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeNearBox(double newValue) {
    nearSlider->setValue(int(newValue*10.f));
    nearBox->setValue(newValue);
    settings.nearPlane = nearBox->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeFarBox(double newValue) {
    farSlider->setValue(int(newValue*10.f));
    farBox->setValue(newValue);
    settings.farPlane = farBox->value();
    realtime->settingsChanged();
}

// Extra Credit:

void MainWindow::onExtraCredit1() {
    settings.extraCredit1 = !settings.extraCredit1;
    realtime->settingsChanged();
}

void MainWindow::onExtraCredit2() {
    settings.extraCredit2 = !settings.extraCredit2;
    realtime->settingsChanged();
}

void MainWindow::onExtraCredit3() {
    settings.extraCredit3 = !settings.extraCredit3;
    if (settings.extraCredit3) {
        ec4->setChecked(false);
        settings.extraCredit4 = false;
        // load custom scene
        settings.volumeFilePath = "resources/test_volumes/foot_256x256x256_uint8.raw";
        realtime->volumeChanged();
    }
}

void MainWindow::onExtraCredit4() {
    settings.extraCredit4 = !settings.extraCredit4;
    if (settings.extraCredit4) {
        ec3->setChecked(false);
        settings.extraCredit3 = false;
        // load pdiddy scene
        settings.volumeFilePath = "resources/test_volumes/skull_256x256x256_uint8.raw";
        realtime->volumeChanged();
    }
}
