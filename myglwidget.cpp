
#include "myglwidget.h"
#include <iostream>
#include <QVector3D>
#include <QtMath>
#include <QElapsedTimer>
#include <QRandomGenerator>



MyGLWidget::MyGLWidget(QWidget *parent): QOpenGLWidget{parent}{

    //create model
    gimbal= new Model();
    sphere= new Model();
    skybox = new Skybox();

}

MyGLWidget::~MyGLWidget()
{
    makeCurrent();

    gimbal->finiGL();
    glDeleteTextures(1, &gw_tex);
    delete gimbal;
    delete m_prog;
    delete sun_prog;
    sphere->finiGL();
    delete sphere;
    delete  skybox;
}

void MyGLWidget::initializeGL(){
    //Emerald
    m_material[0]={QVector3D(0.0215,0.1745,0.0215), //Ambient licht
                   QVector3D(0.07568,0.61424,0.07568),
                   QVector3D(0.633,0.727811,0.633),0.6 };
    //Gold
    m_material[1]={QVector3D(0.24725,0.1995,0.0745),
                   QVector3D(0.75164,0.60648,0.22648),
                   QVector3D(0.628281,0.555802,0.366065),0.4 };
    //white rubber
    m_material[2]={QVector3D(0.05,0.05,0.05),
                   QVector3D(0.5,0.5,0.5),
                   QVector3D(0.7,0.7,0.7), 0.078125 };
    //Silver
    m_material[3]={QVector3D(0.19225,0.19225,0.19225),
                   QVector3D(0.50754,0.50754,0.50754),
                   QVector3D(0.508273,0.508273,0.508273),0.4 };

    ls[0]={QVector3D(2.0, 0.0, 2.0),QVector3D(1.0, 1.0, 0.0),0.8, 0.8, 1.0, 1.0, 0.22, 0.20};
    ls[1]={QVector3D(0.0, 2.0, 2.0),QVector3D(0.0, 0.0, 1.0),0.8, 0.8, 1.0, 1.0, 0.22, 0.20};
    ls[2]={QVector3D(0.0, 0.0, 0.0),QVector3D(1.0, 1.0, 1.0),0.1, 0.1, 0.1, 1.0, 0.7,1.8};
    ls[3]={QVector3D(-2.0, 0.0, -2.0),QVector3D(0.5, 1.0, 0.5),0.8, 0.8,1.0,1.0,0.22,0.20};
    ls[4]={QVector3D(0.25, -1.25, -0.25),QVector3D(1.0, 0.0, 0.0),0.8, 0.8, 1.0, 1.0, 0.22, 0.20};

    skybox->init_skybox();
    //initialize OGLFunctions and test
    bool success = initializeOpenGLFunctions();
    Q_ASSERT(success); Q_UNUSED(success);

    //Load image
    QImage texImg;
    texImg.load(":/gimbal_wood.jpg");
    Q_ASSERT(!texImg.isNull());

    //Create texture object
    glGenTextures(1, &gw_tex);
    glBindTexture(GL_TEXTURE_2D, gw_tex);

    //Fill with pixel data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,
                 texImg.width(), texImg.height(),
                 0, GL_BGRA, GL_UNSIGNED_BYTE, texImg.bits());


    //Set filtering ( interpolation ) options
    //Without these commands, _sampling will return black_
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //Set wrap mode to "repeat"
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Generate Uniform Buer Object
    glGenBuffers(1, &uboLights);
    glBindBuffer(GL_UNIFORM_BUFFER, uboLights);
    glBufferData(GL_UNIFORM_BUFFER, NUM_LS * sizeof(LightSource), nullptr, GL_STATIC_DRAW ); // Set Buffer size, 64 Byte for each LS
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, uboLights);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // Enable back face culling
    //glEnable(GL_CULL_FACE);

    //Create models
    gimbal->initGL(":/gimbal.obj");

    sphere->initGL(":/sphere.obj");


    //create shaderprogramm
    m_prog = new QOpenGLShaderProgram();
    sun_prog = new QOpenGLShaderProgram();

    // addShader() compiles and attaches shader stages for linking
    m_prog->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/sample.vert");
    m_prog->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/sample.frag");
    sun_prog->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex,":/light.vert");
    sun_prog->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment,":/light.frag");

    //Now link attached stages
    m_prog->link();
    sun_prog->link();

    //Abort program if any of these steps failed
    //Q_ASSERT(m_prog->isLinked());
    Q_ASSERT(sun_prog->isLinked());

    //Depth test enable for better display our model
    glEnable(GL_DEPTH_TEST);

    //Passes if the incoming depth value is less than the stored depth value.
    glDepthFunc(GL_LESS);



}

void MyGLWidget::paintGL()
{
    QElapsedTimer timer;
    timer.start();
    // set clear color dunkelgrau
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    //Erase old pixels
    glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);


    QMatrix4x4 p_matrix;
    QMatrix4x4 m_matrix;
    QMatrix4x4 v_matrix;

    p_matrix.perspective(m_FOV,width()/((float)height()),m_Near,m_Far);
    v_matrix.translate(v_matrix.inverted()*m_CameraPos);

    //Bind shaderprogramm for the sun
    sun_prog->bind();

    //Draw Light sources
    for(unsigned int i=0;i<NUM_LS;i++){

        GLfloat rot_lights=timer.msecsSinceReference()*0.0004*(i+1);
        if(i==0){
            m_lightPos.setX(sin(rot_lights)*15);
            m_lightPos.setY(2);
            m_lightPos.setZ(cos(rot_lights)*15);
        }else if (i==1) {
            ls[i].position.setX(sin(rot_lights)*15);
            ls[i].position.setY(-3);
            ls[i].position.setZ(-cos(rot_lights)*15);

        }else if (i==2) {
            ls[i].position.setX(0);
            ls[i].position.setY(0);
            ls[i].position.setZ(0);

        }else if (i==3) {
            ls[i].position.setX(sin(rot_lights)*15);
            ls[i].position.setY(-cos(rot_lights)*15);
            ls[i].position.setZ(cos(rot_lights)*10);
        }else if (i==4) {
            ls[i].position.setX(3);
            ls[i].position.setY(sin(rot_lights)*15);
            ls[i].position.setZ(cos(rot_lights)*15);
        }

        m_matrix.scale(0.09,0.09,0.09);
        m_matrix.translate(ls[i].position);

        // Set MVP matrix
        m_prog->setUniformValue(3,p_matrix*v_matrix*m_matrix);

        //color for they light
        m_prog->setUniformValue(1,ls[i].color);
        m_matrix.setToIdentity();

        //draw light
        sphere->drawElements();
    }


    //Ambient,Diffuse,Specular,Shininess
    for(int i=0;i<NUM_LS;i++){
        ls[i].ka = ((float) m_RotA) / 360.f;
        ls[i].kd = ((float) m_RotB) / 360.f;
        ls[i].ks = ((float) m_RotC) / 360.f;

    }

    //Bind texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->handleCubeMap());

    //Bind ubo
    glBindBuffer(GL_UNIFORM_BUFFER, uboLights);

    //send LS to Shader
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, uboLights);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(ls), ls);

    //Bind shaderprogramm
    m_prog->bind();

    // write unit indices to uniforms
    m_prog->setUniformValue(19, 0);

    //Viewer positon
    m_prog->setUniformValue(8,m_CameraPos);

    //Light position
    m_prog->setUniformValue(9,m_lightPos);




    //shininess
    m_prog->setUniformValue(14,m_material[2].ambient);
    m_prog->setUniformValue(15,m_material[2].diffuse);
    m_prog->setUniformValue(16,m_material[2].specular);
    m_prog->setUniformValue(17,m_material[2].shininess);

    if(!activateAnimation){

        if(cameraOnGimbal){
            v_matrix.translate(v_matrix.inverted()*QVector3D(0.0,0.0,0.0));
            v_matrix.rotate(-m_RotC,1.0,0.0,0.0);
            v_matrix.rotate(-m_RotB,0.0,1.0,0.0);
            v_matrix.rotate(-m_RotA,1.0,0.0,0.0);

        }
        //First gimbal
        m_matrix.scale(0.9,0.9,0.9);
        m_matrix.rotate(m_RotA,1.0,0.0,0.0);
        // Set MVP matrix
        m_prog->setUniformValue(3,p_matrix);
        m_prog->setUniformValue(4,v_matrix);
        m_prog->setUniformValue(5,m_matrix);
        m_prog->setUniformValue(20,m_matrix.normalMatrix());
        //color for one Ring
        m_prog->setUniformValue(6,QVector4D(1.0,0.0,0.0,1.0));
        //draw model
        gimbal->drawElements();

        //Second gimbal
        m_matrix.scale(0.85,0.85,0.85);
        m_matrix.rotate(m_RotB,0.0,1.0,0.0);
        // Set MVP matrix
        m_prog->setUniformValue(3,p_matrix);
        m_prog->setUniformValue(4,v_matrix);
        m_prog->setUniformValue(5,m_matrix);
        m_prog->setUniformValue(20,m_matrix.normalMatrix());
        //color for one Ring
        m_prog->setUniformValue(6,QVector4D(0.0,1.0,0.0,1.0));
        //draw model
        gimbal->drawElements();

        //Ball
        m_matrix.setToIdentity();
        m_matrix.rotate(m_RotA,1.0,0.0,0.0);
        m_matrix.rotate(m_RotB,0.0,1.0,0.0);
        m_matrix.rotate(m_Angle,0.0,0.0,1.0);
        m_matrix.scale(0.05,0.05,0.05);
        m_matrix.translate(0.0,14.9,1.9);
        // Set MVP matrix
        m_prog->setUniformValue(3, p_matrix);
        m_prog->setUniformValue(4,v_matrix);
        m_prog->setUniformValue(5,m_matrix);
        m_prog->setUniformValue(17,m_matrix.normalMatrix());
        //color for they sphere
        m_prog->setUniformValue(6,QVector4D(0.9,0.8,0.7,1.0));
        //draw sphere
        sphere->drawElements();

        //third gimbal
        m_matrix.setToIdentity();
        m_matrix.rotate(m_RotA,1.0,0.0,0.0);
        m_matrix.rotate(m_RotB,0.0,1.0,0.0);
        m_matrix.rotate(m_RotC,1.0,0.0,0.0);
        m_matrix.scale(0.9,0.9,0.9);
        m_matrix.scale(0.85,0.85,0.85);
        m_matrix.scale(0.849,0.849,0.849);
        // Set MVP matrix
        m_prog->setUniformValue(3,p_matrix);
        m_prog->setUniformValue(4,v_matrix);
        m_prog->setUniformValue(5,m_matrix);
        m_prog->setUniformValue(20,m_matrix.normalMatrix());
        //color for one Ring
        m_prog->setUniformValue(6,QVector4D(0.0, 0.0, 1.0,1.0));
        //draw model
        gimbal->drawElements();
    }
    if(activateAnimation){

        if(cameraOnGimbal){
            v_matrix.translate(v_matrix.inverted()*QVector3D(0.0,0.0,0.0));


        }

        //First gimbal
        m_matrix.scale(0.9,0.9,0.9);
        GLfloat rot_first_gimbal=timer.msecsSinceReference()/-12;
        m_matrix.rotate(rot_first_gimbal,1.0,0.0,0.0);
        // Set MVP matrix
        m_prog->setUniformValue(3,p_matrix);
        m_prog->setUniformValue(4,v_matrix);
        m_prog->setUniformValue(5,m_matrix);
        m_prog->setUniformValue(20,m_matrix.normalMatrix());
        //color for one Ring
        m_prog->setUniformValue(6,QVector4D(1.0,0.0,0.0,1.0));
        //draw model
        gimbal->drawElements();

        //Second gimbal
        m_matrix.scale(0.85,0.85,0.85);
        GLfloat rot_second_gimbal=timer.msecsSinceReference()/10;
        m_matrix.rotate(rot_second_gimbal,0.0,1.0,0.0);
        // Set MVP matrix
        m_prog->setUniformValue(3, p_matrix);
        m_prog->setUniformValue(4,v_matrix);
        m_prog->setUniformValue(5,m_matrix);
        m_prog->setUniformValue(20,m_matrix.normalMatrix());
        //color for one Ring
        m_prog->setUniformValue(6,QVector4D(0.0,1.0,0.0,1.0));
        //draw model
        gimbal->drawElements();

        //Ball
        //Variable for the ball rotation
        GLfloat rot_bal=timer.msecsSinceReference();
        m_matrix.setToIdentity();
        m_matrix.rotate(rot_first_gimbal,1.0,0.0,0.0);
        m_matrix.rotate(rot_second_gimbal,0.0,1.0,0.0);
        m_matrix.rotate(rot_bal/10,0.0,0.0,1.0);
        m_matrix.scale(0.05,0.05,0.05);
        m_matrix.translate(0.0,14.9,1.9);
        // Set MVP matrix
        m_prog->setUniformValue(3, p_matrix);
        m_prog->setUniformValue(4,v_matrix);
        m_prog->setUniformValue(5,m_matrix);
        m_prog->setUniformValue(20,m_matrix.normalMatrix());
        //color for they sphere
        m_prog->setUniformValue(6,QVector4D(0.9,0.8,0.7,1.0));
        //draw sphere
        sphere->drawElements();


        //third gimbal
        m_matrix.setToIdentity();
        GLfloat rot_third_gimbal=timer.msecsSinceReference();
        m_matrix.rotate(rot_first_gimbal,1.0,0.0,0.0);
        m_matrix.rotate(rot_second_gimbal,0.0,1.0,0.0);
        m_matrix.rotate(rot_third_gimbal/5,1.0,0.0,0.0);
        if(cameraOnGimbal){
            v_matrix.rotate(-rot_third_gimbal/5,1.0,0.0,0.0);
            v_matrix.rotate(-rot_second_gimbal,0.0,1.0,0.0);
            v_matrix.rotate(-rot_first_gimbal,1.0,0.0,0.0);
        }
        m_matrix.scale(0.9,0.9,0.9);
        m_matrix.scale(0.85,0.85,0.85);
        m_matrix.scale(0.849,0.849,0.849);
        // Set MVP matrix
        m_prog->setUniformValue(3, p_matrix);
        m_prog->setUniformValue(4,v_matrix);
        m_prog->setUniformValue(5,m_matrix);
        m_prog->setUniformValue(20,m_matrix.normalMatrix());
        //color for one Ring
        m_prog->setUniformValue(6,QVector4D(0.0, 0.0, 1.0,1.0));
        //draw model
        gimbal->drawElements();


    }

    //Draw Skybox
    skybox->draw(p_matrix,v_matrix);

    //Request next frame to be drawn
    update();
}

void MyGLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}
void MyGLWidget::setFOV(int value){
    this->m_FOV = value;
    emit vCFOV(value);
}
void MyGLWidget::setAngle(int value){
    this->m_Angle=value;
    emit vCAngle(value);
};
void MyGLWidget::setProjectionMode(bool value){
    this->m_ortho=value;
    emit vCProjectionMode(value);

}
void MyGLWidget::setNear(double value){
    this->m_Near=value;
    emit vCNear(value);
}
void MyGLWidget::setFar(double value){
    this->m_Far=value;
    emit vCFar(value);
}
void MyGLWidget::setRotationA(int value){
    this->m_RotA= value;
    emit vCRotationA(value);
}
void MyGLWidget::setRotationB(int value){
    this->m_RotB=value;
    emit vCRotationB(value);

}
void MyGLWidget::setRotationC(int value){
    this->m_RotC=value;
    emit vCRotationC(value);
}
void MyGLWidget::diffNearFar(double value){
    if(m_Far-m_Near<2.0f){
        emit signalDiffNearFar(value);
    }
}

void MyGLWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Down || event->key() == Qt::Key_S) {

        std::cout<<"x: "<<m_CameraPos.x()<<", y: "<<m_CameraPos.y()<<std::endl;
        m_CameraPos.setX(m_CameraPos.x()-0.2f);
        m_CameraPos.setY(m_CameraPos.y()-0.2f);
        std::cout<<"x: "<<m_CameraPos.x()<<", y: "<<m_CameraPos.y()<<std::endl;
    }
    else if(event->key() == Qt::Key_Up || event->key() == Qt::Key_W) {

        std::cout<<"x: "<<m_CameraPos.x()<<", y: "<<m_CameraPos.y()<<std::endl;
        m_CameraPos.setX(m_CameraPos.x()+0.2f);
        m_CameraPos.setY(m_CameraPos.y()+0.2f);
        std::cout<<"x: "<<m_CameraPos.x()<<", y: "<<m_CameraPos.y()<<std::endl;
    }
    else {
        QOpenGLWidget::keyPressEvent(event);
    }
}
void MyGLWidget::setAnimation(bool value){

    activateAnimation=value;
    if(value)
        qDebug()<<"Animation activate!!";
    else
        qDebug()<<"Animation desctivate!!";
    emit vCAnimation(value);

}
void MyGLWidget::setCameraOnGimbal(bool value){

    cameraOnGimbal=value;
    if(value)
        qDebug()<<"Camera on gimbal activate!!";
    else
        qDebug()<<"Camera on gimbal desctivate!!";
    emit vCCOG(value);

}





















