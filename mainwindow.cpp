#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->openGLWidget->setFocusPolicy(Qt::StrongFocus);

    connect(ui->hsRotationA,&QSlider::valueChanged,ui->openGLWidget,&MyGLWidget::setRotationA);
    connect(ui->hsRotationB,&QSlider::valueChanged,ui->openGLWidget,&MyGLWidget::setRotationB);
    connect(ui->hsRotationC,&QSlider::valueChanged,ui->openGLWidget,&MyGLWidget::setRotationC);

    connect(ui->rbPerspective,&QRadioButton::clicked,ui->openGLWidget,&MyGLWidget::setProjectionMode);
    connect(ui->rbOrthogonal,&QRadioButton::clicked,ui->openGLWidget,&MyGLWidget::setProjectionMode);

    connect(ui->dsbFar,QOverload<double>::of(&QDoubleSpinBox::valueChanged),ui->openGLWidget,&MyGLWidget::setFar);
    connect(ui->dsbNear,QOverload<double>::of(&QDoubleSpinBox::valueChanged),ui->openGLWidget,&MyGLWidget::setNear);

    connect(ui->vsAngle,&QSlider::valueChanged,ui->openGLWidget,&MyGLWidget::setAngle);
    connect(ui->vsFOV,&QSlider::valueChanged,ui->openGLWidget,&MyGLWidget::setFOV);

    connect(ui->vsFOV, &QSlider::valueChanged, [=](double value) {qDebug() << "FOV: " << value;});
    connect(ui->vsAngle, &QSlider::valueChanged, [=](double value) {qDebug() << "Angle: " << value;});
    connect(ui->rbReset,&QPushButton::released, this, &MainWindow::reset);

    connect(ui->dsbFar, QOverload<double>::of(&QDoubleSpinBox::valueChanged),ui->openGLWidget,&MyGLWidget::diffNearFar);
    connect(ui->dsbNear, QOverload<double>::of(&QDoubleSpinBox::valueChanged),ui->openGLWidget,&MyGLWidget::diffNearFar);
    connect(ui->openGLWidget, &MyGLWidget::signalDiffNearFar,this,&MainWindow::resetNear);

    connect(ui->cBAnimation,&QCheckBox::clicked,ui->openGLWidget,&MyGLWidget::setAnimation);
    connect(ui->cBCOG,&QCheckBox::clicked,ui->openGLWidget,&MyGLWidget::setCameraOnGimbal);

    ui->vsFOV->setValue(45);
    ui->dsbFar->setValue(20.1);
    ui->dsbNear->setValue(0.1);



}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::reset(){

        ui->vsFOV->setValue(45);
        ui->vsAngle->setValue(0);
        ui->spAngle->setValue(0);
        ui->spFOV->setValue(45);
        ui->hsRotationA->setValue(0);
        ui->hsRotationB->setValue(0);
        ui->hsRotationC->setValue(0);
        ui->dsbFar->setValue(0);
        ui->dsbNear->setValue(0);
        ui->rbOrthogonal->setChecked(false);
        ui->rbPerspective->setChecked(false);
}

void MainWindow::resetNear(double value){

  ui->dsbFar->setValue(value+2.0);

}
