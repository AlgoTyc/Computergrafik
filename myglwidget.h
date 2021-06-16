#ifndef MYGLWIDGET_H
#define MYGLWIDGET_H

#include <QOpenGLWidget>
#include <QWidget>
#include <QKeyEvent>
#include <QVector3D>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include "model.h"
#include "skybox.h"
#define NUM_LS 5
struct Material {
      QVector3D ambient;
      QVector3D diffuse;
      QVector3D specular;
      float shininess;
};

struct LightSource {
    alignas(16) QVector3D position;
    alignas(16) QVector3D color;
    float ka;
    float kd;
    float ks;
    float constant;
    float linear;
    float quadratic;
};


class MyGLWidget : public QOpenGLWidget,private QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
private:
    bool m_ortho;
    int m_FOV=45;
    int m_Angle;
    double m_Near=0.1;
    double m_Far=20.1;
    int m_RotA;
    int m_RotB;
    int m_RotC;
    QVector3D m_CameraPos=QVector3D(0.0,0.0,-4.0);

    Model* gimbal;
    GLuint gw_tex;
    QOpenGLShaderProgram* m_prog;
    Model* sphere;
    Skybox* skybox;
    bool activateAnimation;
    bool cameraOnGimbal;

    QOpenGLShaderProgram* sun_prog;
    QVector3D m_lightPos=QVector3D(1.0, 1.0, 1.0);
    Material m_material[4];
    LightSource ls[NUM_LS];
    unsigned int uboLights; // ubo Handler


public:
    MyGLWidget(QWidget *parent = 0);
    ~MyGLWidget();

protected:
    void initializeGL()override;
    void paintGL()override;
    void resizeGL(int, int)override;

public slots:
    void setFOV(int);
    void setAngle(int);
    void setProjectionMode(bool);
    void setNear(double);
    void setFar(double);
    void setRotationA(int);
    void setRotationB(int);
    void setRotationC(int);
    void diffNearFar(double);
    void keyPressEvent(QKeyEvent*) override;
    void setAnimation(bool);
    void setCameraOnGimbal(bool);

signals:
    void signalDiffNearFar(double);
    //vC for valueChanged
    void vCFOV(int);
    void vCAngle(int);
    void vCNear(double);
    void vCFar(double);
    void vCProjectionMode(bool);
    void vCRotationA(int);
    void vCRotationB(int);
    void vCRotationC(int);
    void vCAnimation(bool);
    void vCCOG(bool);

};

#endif // MYGLWIDGET_H
