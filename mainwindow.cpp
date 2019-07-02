#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setFocusPolicy(Qt::StrongFocus);

    // FOV Connections
    connect(ui->vsFOV, &QSlider::valueChanged, ui->openGLWidget, &MyGLWidget::setFOV);
    connect(ui->vsFOV, &QSlider::valueChanged, ui->spFOV, &QSpinBox::setValue);
    connect(ui->spFOV, QOverload<int>::of(&QSpinBox::valueChanged), ui->vsFOV, &QSlider::setValue);
    connect(ui->vsFOV, &QSlider::valueChanged, [=]{ qInfo() << "FOV_Lambda:\t\t" << ui->openGLWidget->getFOV(); });

    // Angle Connections
    connect(ui->vsAngle, &QSlider::valueChanged, ui->openGLWidget, &MyGLWidget::setAngle);
    connect(ui->vsAngle, &QSlider::valueChanged, ui->spAngle, &QSpinBox::setValue);
    connect(ui->spAngle, QOverload<int>::of(&QSpinBox::valueChanged), ui->vsAngle, &QSlider::setValue);
    connect(ui->vsAngle, &QSlider::valueChanged, [=]{ qInfo() << "Angle_Lambda:\t" << ui->openGLWidget->getAngle(); });

    // Distance Connections
    connect(ui->dsbNear, QOverload<double>::of(&QDoubleSpinBox::valueChanged), ui->openGLWidget, &MyGLWidget::setNear);
    connect(ui->dsbFar, QOverload<double>::of(&QDoubleSpinBox::valueChanged), ui->openGLWidget, &MyGLWidget::setFar);
    connect(ui->openGLWidget, &MyGLWidget::nearValueChanged, ui->dsbNear, &QDoubleSpinBox::setValue);
    connect(ui->openGLWidget, &MyGLWidget::farValueChanged, ui->dsbFar, &QDoubleSpinBox::setValue);

    // Rotation Connections
    connect(ui->hsRotationA, &QSlider::valueChanged, ui->openGLWidget, &MyGLWidget::setRotationA);
    connect(ui->hsRotationB, &QSlider::valueChanged, ui->openGLWidget, &MyGLWidget::setRotationB);
    connect(ui->hsRotationC, &QSlider::valueChanged, ui->openGLWidget, &MyGLWidget::setRotationC);
    connect(ui->hsRotationT, &QSlider::valueChanged, ui->openGLWidget, &MyGLWidget::setRotationT);
    connect(ui->cbAnimation, &QCheckBox::clicked, ui->openGLWidget, &MyGLWidget::toggleAnimation);
    connect(ui->cbGimbal, &QCheckBox::clicked, ui->openGLWidget, &MyGLWidget::toggleGimbalCamera);

    // Projection Mode Connections
    connect(ui->rbOrthogonal, &QRadioButton::toggled, ui->openGLWidget, &MyGLWidget::setProjectionMode);

    connect(ui->cbDepth, &QCheckBox::clicked, ui->openGLWidget, &MyGLWidget::toggleDepth);
    connect(ui->cbBlur, &QCheckBox::clicked, ui->openGLWidget, &MyGLWidget::toggleBlur);

    connect(ui->hsBlur, &QSlider::valueChanged, ui->openGLWidget, &MyGLWidget::setBlur);

    // PushButton Connections
    connect(ui->rbReset, &QPushButton::released, this, &MainWindow::reset);
    connect(ui->rbPrint, &QPushButton::released, ui->openGLWidget, &MyGLWidget::printAll);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::reset() {
    ui->vsFOV->setValue(1);
    ui->vsAngle->setValue(0);
    ui->dsbNear->setValue(2.0);
    ui->dsbFar->setValue(0);
    ui->hsRotationA->setValue(0);
    ui->hsRotationB->setValue(0);
    ui->hsRotationC->setValue(0);
    ui->hsRotationT->setValue(0);
    ui->rbPerspective->click();
    qInfo() << "------------------------------";
    qInfo() << "All values reset.";
    qInfo() << "------------------------------";
}
