#include "myglwidget.h"
#include "gauss_filter.h"
#include <QDebug>
#include <QKeyEvent>
#include <QtMath>
#include <QDateTime>

MyGLWidget::MyGLWidget(QWidget *parent) : QOpenGLWidget(parent) {
    setFocusPolicy(Qt::StrongFocus);
    debugMode = false;
}

MyGLWidget::~MyGLWidget(){
    finalizeGL();
}

void MyGLWidget::setFOV(int value){
    m_FOV = value;
    if(debugMode) qInfo() << "FOV:\t\t" << m_FOV;
}

void MyGLWidget::setAngle(int value){
    m_Angle = value;
    if(debugMode) qInfo() << "Angle:\t\t" << m_Angle;
    updateProjectionMatrix();
}

void MyGLWidget::setProjectionMode(){
    m_ProjectionMode = !m_ProjectionMode;
    if(debugMode) qInfo() << "ProjectionMode " << (m_ProjectionMode ? "Orthogonal" : "Perspective");
}

void MyGLWidget::setNear(double value){
    if(value != m_Far){
        m_Near = value;
    }
    emit nearValueChanged(m_Near);
    if(m_Near > m_Far && m_Near - m_Far < 2.0f){
        if(m_Far == 0){
            m_Near = 2;
            emit nearValueChanged(m_Near);
        } else {
            m_Far = m_Near - 2;
            emit farValueChanged(m_Far);
        }
    } else if(m_Near < m_Far && m_Far - m_Near < 2.0f){
        m_Far = m_Near + 2;
        emit farValueChanged(m_Far);
    }
    if(debugMode) qInfo() << "Near:\t\t" << m_Near;
    updateProjectionMatrix();
}

void MyGLWidget::setFar(double value){
    if(value != m_Near){
        m_Far = value;
    }
    emit farValueChanged(m_Far);
    if(m_Near > m_Far && m_Near - m_Far < 2.0f){
        m_Near = m_Far + 2;
        emit nearValueChanged(m_Near);
    } else if(m_Near < m_Far && m_Far - m_Near < 2.0f){
        if(m_Near == 0){
            m_Far = 2;
            emit farValueChanged(m_Far);
        } else {
            m_Near = m_Far - 2;
            emit nearValueChanged(m_Near);
        }
    }
    if(debugMode) qInfo() << "Far:\t\t" << m_Far;
    updateProjectionMatrix();
}

void MyGLWidget::setRotationA(float value){
    gimbalA.rotate(value - m_RotationA, rotAxisA);
    gimbalB.rotate(value - m_RotationA, rotAxisA);
    gimbalC.rotate(value - m_RotationA, rotAxisA);

    rotAxisB = gimbalA.transposed() * QVector3D(1, 0, 0);
    rotAxisC = gimbalB.transposed() * QVector3D(0, 1, 0);

    m_RotationA = value;
    m_Ambient = (float)value/360;
    if(debugMode) qInfo() << "RotationA:\t\t" << m_RotationA;
}

void MyGLWidget::setRotationB(float value){
    gimbalB.rotate(value - m_RotationB, rotAxisB);
    gimbalC.rotate(value - m_RotationB, rotAxisB);

    rotAxisC = gimbalB.transposed() * QVector3D(0, 1, 0);

    m_RotationB = value;
    m_Diffuse = (float)value/360;

    if(debugMode) qInfo() << "RotationB:\t\t" << m_RotationB;
}

void MyGLWidget::setRotationC(float value){
    gimbalC.rotate(value - m_RotationC, rotAxisC);
    m_RotationC = value;
    m_Specular = (float)value/360;
    if(debugMode) qInfo() << "RotationC:\t\t" << m_RotationC;
}

void MyGLWidget::setRotationT(int value){
    m_RotationT = value;
    if(debugMode) qInfo() << "RotationT:\t\t" << m_RotationT;
    mp_program->bind();
    mp_program->setUniformValue(mp_program->uniformLocation("uMove"), float (m_RotationT/100.0));
}

void MyGLWidget::setBlur(int value) {
    m_BlurLevel = value - value % 2 + 1;
    qInfo() << "Blur: " << m_BlurLevel;
}

void MyGLWidget::toggleAnimation(bool animate) {
    m_Animate = animate;
}

void MyGLWidget::toggleGimbalCamera(bool gimbalCam){
    m_GimbalCam = gimbalCam;
    if(m_GimbalCam){
        m_Near = 5.0;
        m_Far = 7.0;
        m_Angle = 180;
        setAngle(180);
        setNear(5.0);
        setFar(7.0);
    } else {
        m_Near = 0.0;
        m_Far = 2.0;
        m_Angle = 0;
        setNear(0.0);
        setFar(2.0);
        setAngle(0);
        cameraMat = QMatrix4x4();
    }
}

void MyGLWidget::toggleDepth(bool depthActive) {
    m_Depth = depthActive;
}

void MyGLWidget::toggleBlur(bool blurActive) {
    m_Blur = blurActive;
}

void MyGLWidget::updateProjectionMatrix() {
    qInfo() << "FOV: " << m_FOV << "Near: " << m_Near << "Far: " << m_Far;
    projectionMat = QMatrix4x4();
    projectionMat.perspective(qDegreesToRadians((double)m_Angle), m_Aspect, (float) m_Near, (float) m_Far);
    qInfo() << projectionMat;
}

void MyGLWidget::updateCamera(){
   /* cameraMat.setRow(0, {1, 0, 0, m_CameraPos[2]});
    cameraMat.setRow(1, {0, 1, 0, m_CameraPos[0]});
    cameraMat.setRow(2, {0, 0, 1, 0});
    cameraMat.scale(m_CameraPos[1], m_CameraPos[1], m_CameraPos[1]);
    qInfo() << "Scale: " << m_CameraPos[1];
    qInfo() << cameraMat.transposed();*/
    if(debugMode) qInfo() << "CameraPos:\t" << m_CameraPos;
    cameraMat = QMatrix4x4();
    cameraMat.translate(-m_CameraPos[1], -m_CameraPos[0], -m_CameraPos[2]);
    cameraMat.rotate(m_CameraAngle.x(), {0, 1, 0});
    cameraMat.rotate(m_CameraAngle.y(), {1, 0, 0});
    //cameraMat = gimbalC;
    /*cameraMat.lookAt(m_CameraPos,
                    {0, 0, 0},
                    {0, 1, 0}
                     );
    cameraMat = cameraMat.transposed();*/
    /*cameraMat = cameraMat.transposed();
    cameraMat.translate(-m_CameraPos[1], -m_CameraPos[0], 0);
    cameraMat.scale(m_CameraPos[2]);
    cameraMat.rotate(m_CameraPos[2], {0, 1, 0});*/
}

void MyGLWidget::printAll(){
    qInfo() << "------------------------------";
    qInfo() << "FOV:\t\t" << m_FOV;
    qInfo() << "Angle:\t\t" << m_Angle;
    qInfo() << "Projection Mode:\t" <<  (m_ProjectionMode ? "Orthogonal" : "Perspective");
    qInfo() << "Near:\t\t" << m_Near;
    qInfo() << "Far:\t\t" << m_Far;
    qInfo() << "RotationA:\t\t" << m_RotationA;
    qInfo() << "RotationB:\t\t" << m_RotationB;
    qInfo() << "RotationC:\t\t" << m_RotationC;
    qInfo() << "CameraPos:\t\t" << m_CameraPos;
    qInfo() << "------------------------------";
}

int MyGLWidget::getFOV()    { return m_FOV; }
int MyGLWidget::getAngle()  { return m_Angle; }

void MyGLWidget::keyPressEvent(QKeyEvent *event){
    float cameraSpeed = 0.02f;
    if(event->key() == Qt::Key_E) {
        m_CameraPos.setX(m_CameraPos.x() + cameraSpeed);
        updateCamera();
    } else if(event->key() == Qt::Key_Q) {
        m_CameraPos.setX(m_CameraPos.x() - cameraSpeed);
        updateCamera();
    } else if(event->key() == Qt::Key_A) {
        m_CameraPos.setY(m_CameraPos.y() - cameraSpeed);
        updateCamera();
    } else if(event->key() == Qt::Key_D) {
        m_CameraPos.setY(m_CameraPos.y() + cameraSpeed);
        updateCamera();
    } else if(event->key() == Qt::Key_S) {
        m_CameraPos.setZ(m_CameraPos.z() - cameraSpeed);
        updateCamera();
    } else if(event->key() == Qt::Key_W) {
        m_CameraPos.setZ(m_CameraPos.z() + cameraSpeed);
        updateCamera();
    } else if(event->key() == Qt::Key_Left) {
        m_CameraAngle.setX(m_CameraAngle.x() - cameraSpeed*100);
        updateCamera();
    } else if(event->key() == Qt::Key_Right) {
        m_CameraAngle.setX(m_CameraAngle.x() + cameraSpeed*100);
        updateCamera();
    } else if(event->key() == Qt::Key_Down) {
        m_CameraAngle.setY(m_CameraAngle.y() - cameraSpeed*100);
        updateCamera();
    } else if(event->key() == Qt::Key_Up) {
        m_CameraAngle.setY(m_CameraAngle.y() + cameraSpeed*100);
        updateCamera();
    } else if(event->key() == Qt::Key_F12) {
        // size() returns the size of the widget, it is assumed to be the same as the texture size
        QImage img(size(), QImage::Format_RGBA8888);
        // make/doneCurrent() are only needed when calling from outside paintGL()
        makeCurrent();
        glBindTexture(GL_TEXTURE_2D, colorTex);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, img.bits());
        doneCurrent();
        //QDateTime curTime(QDateTime::currentDateTime());
        //QString filename = "Screenshot_" + QString(curTime.toTime_t()) + ".png";
        //qInfo() << "Screenshot gespeichert (" << filename << ")";
        img.mirrored(false, true).save("Screenshot.png");
    } else {
        QOpenGLWidget::keyPressEvent(event);
    }
}

void MyGLWidget::initializeGL(){
    Q_ASSERT(initializeOpenGLFunctions());

    m_Aspect = this->width()/this->height();
    //setAngle(45);
    /*setNear(5.0);
    setFar(7.0);*/

    // Perspective-Hilfsfunktion: FOV, Aspect, Near, Far
    projectionMat.perspective(m_Angle, m_Aspect, m_Near, m_Far);

    // Dreiecke erzeugen für Anzeige der Tiefentextur
    struct Vertex {
        GLfloat position[2];
        GLfloat texture[2];
    };

    Vertex vert[] = {
        {
            {-1, -1},
            {0, 0}
        },
        {
            {1, -1},
            {1, 0}
        },
        {
            {-1, 1},
            {0, 1}
        },
        {
            {1, 1},
            {1, 1}
        }
    };

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    GLuint data[] = {0, 1, 2, 1, 3, 2};
    glGenBuffers(1, &m_ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);

    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vert), vert, GL_STATIC_DRAW);

    #define OFS(s, a) reinterpret_cast<void* const>(offsetof(s, a))

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), OFS(Vertex,position));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), OFS(Vertex,texture));

    #undef OFS

    rotAxisA = {0, 1, 0};
    rotAxisB = {1, 0, 0};
    rotAxisC = {0, 1, 0};

    gimbalA.scale(0.6);
    gimbalB.scale(.45);
    gimbalC.scale(.3);
    gimbalA.scale(.7);
    gimbalB.scale(.7);
    gimbalC.scale(.7);
    lightModel1.scale(.03);
    lightModel2.scale(.03);
    lightModel3.scale(.03);
    lightModel4.scale(.03);
    lightModel5.scale(.03);
    //lightModel1.translate(0,0,-10);

    gimbal.initGL(":/gimbal.obj");
    sphere.initGL(":/sphere.obj");
    light.initGL(":/sphere.obj");

    // Lichter initialisieren
    lightModel1.translate(lightPos1);
    lightModel2.translate(lightPos2);
    lightModel3.translate(lightPos3);
    lightModel4.translate(lightPos4);
    lightModel5.translate(lightPos5);

    // Gelbes Licht
    ls[0].position = lightPos1;
    ls[0].color = {1.0f, 1.0f, 0.0f};
    ls[0].ka = 0.8;
    ls[0].kd = 0.8;
    ls[0].ks = 1.0;
    ls[0].constant = 1.0;
    ls[0].linear = 0.22;
    ls[0].quadratic = 0.20;

    // Blaues Licht
    ls[1].position = lightPos2;
    ls[1].color = {0.0f, 0.0f, 1.0f};
    ls[1].ka = 0.8;
    ls[1].kd = 0.8;
    ls[1].ks = 1.0;
    ls[1].constant = 1.0;
    ls[1].linear = 0.22;
    ls[1].quadratic = 0.20;

    // Weißes Licht
    ls[2].position = lightPos3;
    ls[2].color = {.3f, .3f, .3f};
    ls[2].ka = 0.1;
    ls[2].kd = 0.1;
    ls[2].ks = 0.1;
    ls[2].constant = 1.0;
    ls[2].linear = 0.7;
    ls[2].quadratic = 1.8;

    // Lila Licht
    ls[3].position = lightPos4;
    ls[3].color = {0.65f, 0.3f, 1.0f};
    ls[3].ka = 0.8;
    ls[3].kd = 0.8;
    ls[3].ks = 1.0;
    ls[3].constant = .2;
    ls[3].linear = 0.22;
    ls[3].quadratic = 0.20;

    // Rotes Licht
    ls[4].position = lightPos5;
    ls[4].color = {1.0f, 0.0f, 0.0f};
    ls[4].ka = 0.8;
    ls[4].kd = 0.8;
    ls[4].ks = 1.0;
    ls[4].constant = 1.0;
    ls[4].linear = 0.22;
    ls[4].quadratic = 0.20;

    glGenBuffers(1, &uboLights);
    glBindBuffer(GL_UNIFORM_BUFFER, uboLights);
    glBufferData(GL_UNIFORM_BUFFER, NUM_LS * sizeof(LightSource), nullptr, GL_STATIC_DRAW ); // Set Buer size, 64 Byte for each LS
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, uboLights);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // Framebuffer erstellen und binden
    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    // Farbtextur erstellen und binden
    glGenTextures(1, &colorTex);
    glBindTexture(GL_TEXTURE_2D, colorTex);

    // Speicher für Farbtextur allozieren
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, this->width(), this->height());
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, this->width(), this->height(), 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);

    // Tiefentextur erstellen und binden
    glGenTextures(1, &depthTex);
    glBindTexture(GL_TEXTURE_2D, depthTex);

    // Speicher für Tiefentextur allozieren
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT16, this->width(), this->height());

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTex, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTex, 0);

    Q_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

    // Textur aus Datei in QImage laden
    QImage texImg;
    texImg.load(":/metal_texture.jpg");
    Q_ASSERT(!texImg.isNull());

    // Textur-Objekt erstellen
    glGenTextures(1, &m_tex);
    glBindTexture(GL_TEXTURE_2D, m_tex);

    // Textur-Modus auf Repeat stellen
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Mit Textur füllen
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, texImg.width(), texImg.height(),
    0, GL_BGRA, GL_UNSIGNED_BYTE, texImg.bits());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    //glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    skybox.initializeGL();

    // Shader Programm für texturiertes Objekt
    mp_program = new QOpenGLShaderProgram();
    mp_program->addShaderFromSourceFile(QOpenGLShader::Vertex,      ":/default.vert");
    mp_program->addShaderFromSourceFile(QOpenGLShader::Fragment,    ":/default.frag");
    Q_ASSERT(mp_program->link());

    mp_program2 = new QOpenGLShaderProgram();
    mp_program2->addShaderFromSourceFile(QOpenGLShader::Vertex,      ":/depth.vert");
    mp_program2->addShaderFromSourceFile(QOpenGLShader::Fragment,      ":/depth.frag");
    Q_ASSERT(mp_program2->link());

    mp_program_light = new QOpenGLShaderProgram();
    mp_program_light->addShaderFromSourceFile(QOpenGLShader::Vertex,      ":/light.vert");
    mp_program_light->addShaderFromSourceFile(QOpenGLShader::Fragment,    ":/light.frag");
    Q_ASSERT(mp_program_light->link());

    mp_program_comp = new QOpenGLShaderProgram();
    mp_program_comp->addShaderFromSourceFile(QOpenGLShader::Compute, ":/compute.glsl");
    Q_ASSERT(mp_program_comp->link());

    //qInfo() << generate1DGauss(15);
}

void MyGLWidget::resizeGL(int w, int h){
    m_Aspect = (double)w/(double)h;
    qInfo() << "W: " << w << "\tH: " << h << "\tRatio: " << m_Aspect;
    glViewport(0, 0, this->width(), this->height());

    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    glGenTextures(1, &colorTex);
    glBindTexture(GL_TEXTURE_2D, colorTex);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, w, h);

    glGenTextures(1, &outTex);
    glBindTexture(GL_TEXTURE_2D, outTex);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, w, h);

    glGenTextures(1, &depthTex);
    glBindTexture(GL_TEXTURE_2D, depthTex);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT16, w, h);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTex, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTex, 0);
    Q_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

    updateProjectionMatrix();
}

void MyGLWidget::paintGL(){
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    skybox.render(projectionMat.transposed(), cameraMat);

    glBindBuffer(GL_UNIFORM_BUFFER, uboLights); //Bind Buffer
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, uboLights);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(ls), ls); // Übergebe LS an den Shader

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.getHandle());

    // Color-Shaderprogramm Binden, Zeichnen und Releasen
    mp_program->bind();
    //mp_program->setUniformValue(mp_program->uniformLocation("uLightPosition"), lightPos);
    mp_program->setUniformValue(mp_program->uniformLocation("uViewPosition"), cameraMat);
    mp_program->setUniformValue(mp_program->uniformLocation("uKa"), (float)m_Ambient);
    mp_program->setUniformValue(mp_program->uniformLocation("uKd"), (float)m_Diffuse);
    mp_program->setUniformValue(mp_program->uniformLocation("uKs"), (float)m_Specular);
    mp_program->setUniformValue(mp_program->uniformLocation("uShininess"), m_FOV);
    mp_program->setUniformValue(mp_program->uniformLocation("uProjection"), projectionMat.transposed());
    mp_program->setUniformValue(mp_program->uniformLocation("uView"), cameraMat);
    mp_program->setUniformValue(mp_program->uniformLocation("uModel"), gimbalA.transposed());
    //glDrawElements(GL_TRIANGLES, loader.lengthOfIndexArray(), GL_UNSIGNED_INT, nullptr);
    // Gimbal-Material: Gold
    //QVector3D ambientMaterial = QVector3D(0.24725, 0.1995, 0.0745);
    //QVector3D diffuseMaterial = QVector3D(0.75164, 0.60648, 0.22648);
    //QVector3D specularMaterial = QVector3D(0.628281, 0.555802, 0.366065);
    QVector3D ambientMaterial = QVector3D(0.25, 0.25, 0.25);
    QVector3D diffuseMaterial = QVector3D(0.4, 0.4, 0.4);
    QVector3D specularMaterial = QVector3D(0.774597, 0.774597, 0.774597);
    float shininess = 0.9;
    int structLocation = 100;
    mp_program->setUniformValue(structLocation, ambientMaterial);
    mp_program->setUniformValue(structLocation + 1, diffuseMaterial);
    mp_program->setUniformValue(structLocation + 2, specularMaterial);
    mp_program->setUniformValue(structLocation + 3, shininess);
    gimbal.drawElements();
    mp_program->setUniformValue(mp_program->uniformLocation("uModel"), gimbalB.transposed());
    //glDrawElements(GL_TRIANGLES, loader.lengthOfIndexArray(), GL_UNSIGNED_INT, nullptr);
    gimbal.drawElements();
    mp_program->setUniformValue(mp_program->uniformLocation("uModel"), gimbalC.transposed());
    gimbal.drawElements();
    ballA = gimbalB.transposed();
    ballA.scale(1/.45);
    ballA.rotate(++counter,{0,0,1});
    ballA.translate({0,.52,0});
    ballA.scale(0.03);
    ballA.rotate(counter * 3.5,{0,0,1});

    // Ball-Material: Schwarzer Gummi
    ambientMaterial = QVector3D(0.02, 0.02, 0.02);
    diffuseMaterial = QVector3D(0.01, 0.01, 0.01);
    specularMaterial = QVector3D(0.4, 0.4, 0.4);
    shininess = 0.078125;
    mp_program->setUniformValue(structLocation, ambientMaterial);
    mp_program->setUniformValue(structLocation + 1, diffuseMaterial);
    mp_program->setUniformValue(structLocation + 2, specularMaterial);
    mp_program->setUniformValue(structLocation + 3, shininess);
    mp_program->setUniformValue(mp_program->uniformLocation("uModel"), ballA);
    sphere.drawElements();
    //glDrawElements(GL_TRIANGLES, loader.lengthOfIndexArray(), GL_UNSIGNED_INT, nullptr);
    mp_program->release();

    mp_program_light->bind();
    mp_program_light->setUniformValue(mp_program_light->uniformLocation("uProjection"), projectionMat.transposed());
    mp_program_light->setUniformValue(mp_program_light->uniformLocation("uView"), cameraMat);

    // Licht 1 rendern
    mp_program_light->setUniformValue(mp_program_light->uniformLocation("uModel"), lightModel1);
    mp_program_light->setUniformValue(mp_program_light->uniformLocation("uLightColor"), ls[0].color);
    light.drawElements();

    // Licht 2 rendern
    mp_program_light->setUniformValue(mp_program_light->uniformLocation("uModel"), lightModel2);
    mp_program_light->setUniformValue(mp_program_light->uniformLocation("uLightColor"), ls[1].color);
    light.drawElements();

    // Licht 3 rendern
    mp_program_light->setUniformValue(mp_program_light->uniformLocation("uModel"), lightModel3);
    mp_program_light->setUniformValue(mp_program_light->uniformLocation("uLightColor"), ls[2].color);
    light.drawElements();

    // Licht 4 rendern
    mp_program_light->setUniformValue(mp_program_light->uniformLocation("uModel"), lightModel5);
    mp_program_light->setUniformValue(mp_program_light->uniformLocation("uLightColor"), ls[3].color);
    light.drawElements();

    // Licht 5 rendern
    mp_program_light->setUniformValue(mp_program_light->uniformLocation("uModel"), lightModel4);
    mp_program_light->setUniformValue(mp_program_light->uniformLocation("uLightColor"), ls[4].color);
    light.drawElements();

    // Licht 1 animieren
    lightModel1 = lightModel1.transposed();
    lightModel1.rotate(1.8, {0,1,0});
    lightModel1 = lightModel1.transposed();
    lightPos1.setX(lightModel1.row(0)[3]);
    lightPos1.setZ(lightModel1.row(2)[3]);

    // Licht 2 animieren
    lightModel2 = lightModel2.transposed();
    lightModel2.rotate(1.8, {1,0,0});
    lightModel2 = lightModel2.transposed();
    lightPos2.setY(lightModel2.row(1)[3]);
    lightPos2.setZ(lightModel2.row(2)[3]);

    // Licht 4 animieren
    lightModel4 = lightModel4.transposed();
    lightModel4.rotate(1.8, {0,1,0});
    lightModel4 = lightModel4.transposed();
    lightPos4.setX(lightModel4.row(0)[3]);
    lightPos4.setZ(lightModel4.row(2)[3]);

    // Licht 5 animieren
    lightModel5 = lightModel5.transposed();
    lightModel5.rotate(1.8, {0,1,0});
    lightModel5 = lightModel5.transposed();
    lightPos5.setX(lightModel5.row(0)[3]);
    lightPos5.setZ(lightModel5.row(2)[3]);

    // Schicke neue Position an Shader
    ls[0].position = lightPos1;
    ls[1].position = lightPos2;
    ls[2].position = lightPos3;
    ls[3].position = lightPos5;
    ls[4].position = lightPos4;
    mp_program_light->release();

    if(m_Animate) {
        setRotationA(m_RotationA + .25);
        setRotationB(m_RotationB + .5);
        setRotationC(m_RotationC + .75);
        //setRotationT(m_RotationT + 1);
    }

    if(m_GimbalCam){
        cameraMat = gimbalC;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, this->defaultFramebufferObject());

    if(m_Depth) {
        mp_program->release();
        glBindVertexArray(m_vao);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        mp_program2->bind();
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depthTex);
        mp_program2->setUniformValue(mp_program2->uniformLocation("uTex"), 1);
        /*glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, colorTex);
        mp_program2->setUniformValue(mp_program2->uniformLocation("uTex2"), 2);*/
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        mp_program2->release();
    } else {
        if(m_Blur) {
            mp_program->release();
            mp_program_comp->bind();

            // m_BlurLevel = 15;
            mp_program_comp->setUniformValue(mp_program_comp->uniformLocation("gLevel"), m_BlurLevel);

            std::vector<double> gaussVal = generate1DGauss(m_BlurLevel);
            float gaussValues[m_BlurLevel];
            for(int i = 0; i < m_BlurLevel; i++) {
                gaussValues[i] = gaussVal[i];
            }
            mp_program_comp->setUniformValueArray(mp_program_comp->uniformLocation("gVec"), gaussValues, m_BlurLevel, 1);

            mp_program_comp->setUniformValue(mp_program_comp->uniformLocation("hBlur"), true);
            glBindImageTexture(0, colorTex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
            glBindImageTexture(1, outTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);
            glDispatchCompute(this->width(), this->height(), 1);
            mp_program_comp->setUniformValue(mp_program_comp->uniformLocation("hBlur"), false);
            glBindImageTexture(0, outTex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
            glBindImageTexture(1, blurTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);
            glDispatchCompute(this->width(), this->height(), 1);

            mp_program_comp->release();
            mp_program2->bind();
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, blurTex);
            mp_program2->setUniformValue(mp_program2->uniformLocation("uTex"), 1);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
            mp_program2->release();
        }
        glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
        glBlitFramebuffer(0, 0, this->width(), this->height(), 0, 0, this->width(), this->height(), GL_COLOR_BUFFER_BIT, GL_LINEAR);
    }

    this->update();
}

void MyGLWidget::finalizeGL() {
    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_ibo);
    glDeleteFramebuffers(1, &m_fbo);
    glDeleteVertexArrays(1, &m_vao);
    glDeleteTextures(1, &m_tex);
    delete mp_program;
}
