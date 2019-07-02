#ifndef MYGLWIDGET_H
#define MYGLWIDGET_H
#define NUM_LS 5

#include <QWidget>
#include <QOpenGLWidget>
#include <QVector3D>
#include <QElapsedTimer>
#include <QOpenGLFunctions_4_4_Core>
#include <QOpenGLShaderProgram>
#include "model.h"
#include "myskybox.h"

class MyGLWidget : public QOpenGLWidget, QOpenGLFunctions_4_4_Core {
    Q_OBJECT
public:
    MyGLWidget(QWidget *parent);
    ~MyGLWidget();
    int getFOV();
    int getAngle();
    void keyPressEvent(QKeyEvent *event);

public slots:
    void setFOV(int value);
    void setAngle(int value);
    void setProjectionMode();
    void setNear(double value);
    void setFar(double value);
    void setRotationA(float value);
    void setRotationB(float value);
    void setRotationC(float value);
    void setRotationT(int value);
    void setBlur(int value);
    void updateProjectionMatrix();
    void updateCamera();
    void printAll();
    void toggleAnimation(bool animate);
    void toggleGimbalCamera(bool gimbalCam);
    void toggleDepth(bool depthActive);
    void toggleBlur(bool blurActive);

signals:
    void nearValueChanged(double value);
    void farValueChanged(double value);

private:
    int m_FOV = 45;
    int m_Angle = 0;
    int m_RotationA;
    int m_RotationB;
    int m_RotationC;
    int m_RotationT;
    int m_BlurLevel = 3;
    int counter;
    float counter2 = .2;
    bool location;
    double m_Aspect;
    double m_Near = 2.0;
    double m_Far = 0.0;
    double m_Ambient = 0.0;
    double m_Diffuse = 0.0;
    double m_Specular = 0.0;
    bool m_ProjectionMode;
    bool m_Depth;
    QVector3D m_CameraPos;
    QVector2D m_CameraAngle;
    bool debugMode;
    bool m_Animate;
    bool m_GimbalCam;
    bool m_Blur;
    QOpenGLShaderProgram* mp_program;
    QOpenGLShaderProgram* mp_program2;
    QOpenGLShaderProgram* mp_program_light;
    QOpenGLShaderProgram* mp_program_comp;
    GLuint m_tex;
    GLuint m_vbo;
    GLuint m_vao;
    GLuint m_ibo;
    GLuint m_fbo;
    GLuint m_tex_ball;
    GLuint m_vbo_ball;
    GLuint m_vao_ball;
    GLuint m_ibo_ball;
    GLuint fbo, colorTex, depthTex, outTex, blurTex;
    unsigned int uboLights;
    struct LightSource {
        QVector3D position;
        float pad0;
        QVector3D color;
        float pad1;
        float ka;
        float kd;
        float ks;
        float pad2;
        float constant;
        float linear;
        float quadratic;
        float pad3;
    };
    LightSource ls[NUM_LS];
    QVector3D rotAxisA;
    QVector3D rotAxisB;
    QVector3D rotAxisC;
    QVector3D rotAxis = {0, 1, 0};
    QVector3D lightPos1 = QVector3D(8.0, 0.0, 8.0) * 2;
    QVector3D lightPos2 = QVector3D(0.0, 8.0, 8.0) * 2;
    QVector3D lightPos3 = QVector3D(0.0, 0.0, 0.0) * 2;
    QVector3D lightPos4 = QVector3D(-8.0, 0.0, -8.0) * 2;
    QVector3D lightPos5 = QVector3D(8.0, -8.0, -1.0) * 2;
    QMatrix4x4 rotMat;
    QMatrix4x4 projectionMat;
    QMatrix4x4 modelViewMat;
    QMatrix4x4 cameraMat;
    QMatrix4x4 gimbalA;
    QMatrix4x4 gimbalB;
    QMatrix4x4 gimbalC;
    QMatrix4x4 lightModel1;
    QMatrix4x4 lightModel2;
    QMatrix4x4 lightModel3;
    QMatrix4x4 lightModel4;
    QMatrix4x4 lightModel5;
    QMatrix4x4 ballA;
    Model gimbal, sphere, light;
    MySkybox skybox;
    QElapsedTimer timer;

protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
    void finalizeGL();
};

#endif // MYGLWIDGET_H
