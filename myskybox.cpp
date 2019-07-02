#include "myskybox.h"
#include <QImage>

MySkybox::MySkybox() {

}

MySkybox::~MySkybox() {
    finalizeGL();
}

void MySkybox::finalizeGL() {
    glDeleteTextures(1, &m_cubeTex);
}

void MySkybox::initializeGL() {
    Q_ASSERT(initializeOpenGLFunctions());



    static const GLfloat vertexData[] = {
        -1, 1, -1,
        -1, -1, -1,
        1, -1, -1,
        1, 1, -1,
        -1, -1, 1,
        -1, 1, 1,
        1, -1, 1,
        1, 1, 1
    };

    static const GLuint indicesData[] = {
        0, 1, 2, 2, 3, 0,
        4, 1, 0, 0, 5, 4,
        2, 6, 7, 7, 3, 2,
        4, 5, 7, 7, 6, 4,
        0, 3, 7, 7, 5, 0,
        1, 4, 2, 2, 4, 6
    };

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(1,&m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER,m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData,GL_STATIC_DRAW);

    glGenBuffers(1, &m_ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesData), indicesData, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,sizeof(GLfloat) * 3, (void*)0);

    glBindVertexArray(0);

    /*#define OFS(s, a) reinterpret_cast<void* const>(offsetof(s, a))

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), OFS(Vertex,position));

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), OFS(Vertex,normals));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), OFS(Vertex,texturePos));

    #undef OFS*/

    // Skybox bauen
    glGenTextures(1, &m_cubeTex);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubeTex);

    // Skybox rechts
    QImage rImg(":/skybox_images/right.jpg");
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA,
    rImg.width(), rImg.height(), 0, GL_BGRA, GL_UNSIGNED_BYTE,
    reinterpret_cast<void const*>(rImg.constBits()));

    // Skybox links
    QImage lImg(":/skybox_images/left.jpg");
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA,
    lImg.width(), lImg.height(), 0, GL_BGRA, GL_UNSIGNED_BYTE,
    reinterpret_cast<void const*>(lImg.constBits()));

    // Skybox oben
    QImage uImg(":/skybox_images/top.jpg");
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA,
    uImg.width(), uImg.height(), 0, GL_BGRA, GL_UNSIGNED_BYTE,
    reinterpret_cast<void const*>(uImg.constBits()));

    // Skybox unten
    QImage dImg(":/skybox_images/bottom.jpg");
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA,
    dImg.width(), dImg.height(), 0, GL_BGRA, GL_UNSIGNED_BYTE,
    reinterpret_cast<void const*>(dImg.constBits()));

    // Skybox hinten
    QImage bImg(":/skybox_images/back.jpg");
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA,
    bImg.width(), bImg.height(), 0, GL_BGRA, GL_UNSIGNED_BYTE,
    reinterpret_cast<void const*>(bImg.constBits()));

    // Skybox vorne
    QImage fImg(":/skybox_images/front.jpg");
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA,
    fImg.width(), fImg.height(), 0, GL_BGRA, GL_UNSIGNED_BYTE,
    reinterpret_cast<void const*>(fImg.constBits()));

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    mp_program = new QOpenGLShaderProgram();
    mp_program->addShaderFromSourceFile(QOpenGLShader::Vertex,      ":/skybox.vert");
    mp_program->addShaderFromSourceFile(QOpenGLShader::Fragment,    ":/skybox.frag");
    Q_ASSERT(mp_program->link());
}

void MySkybox::render(const QMatrix4x4 &projection, QMatrix4x4 view) {
    glDepthMask(GL_FALSE);
    //view.column(3) = QVector4D(0.0, 0.0, 0.0, 0.0);
    //view.scale(10.0);
    glBindVertexArray(m_vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubeTex);

    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);

    mp_program->bind();
    mp_program->setUniformValue(mp_program->uniformLocation("uProjection"), projection);
    mp_program->setUniformValue(mp_program->uniformLocation("uView"), view);
    mp_program->setUniformValue(mp_program->uniformLocation("uSkybox"),0);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);

    mp_program->release();
    glDepthMask(GL_TRUE);

    //glDisable(GL_CULL_FACE);
}


