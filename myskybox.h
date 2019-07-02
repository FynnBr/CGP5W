#ifndef MYSKYBOX_H
#define MYSKYBOX_H

#include <QOpenGLFunctions_4_4_Core>
#include <QOpenGLShaderProgram>

class MySkybox : QOpenGLFunctions_4_4_Core {

public:
    MySkybox();
    ~MySkybox();
    void initializeGL();
    void render(const QMatrix4x4 &projection, QMatrix4x4 view);
    void finalizeGL();
    GLuint getHandle() { return m_cubeTex; }


private:
    GLuint m_cubeTex;
    QOpenGLShaderProgram* mp_program;
    GLuint m_vbo;
    GLuint m_vao;
    GLuint m_ibo;

};

#endif // MYSKYBOX_H
