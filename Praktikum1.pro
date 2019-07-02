#-------------------------------------------------
#
# Project created by QtCreator 2019-04-09T22:29:57
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Praktikum1
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        mainwindow.cpp \
    model.cpp \
    myglwidget.cpp \
    main.cpp \
    myskybox.cpp

HEADERS += \
    gauss_filter.h \
        mainwindow.h \
    model.h \
    myglwidget.h \
    modelloader.h \
    myskybox.h

win32 {
    LIBS += -L../assimp-mingw32-4.1.0/bin -lassimp
    INCLUDEPATH += ../assimp-mingw32-4.1.0/include
}


FORMS += \
        mainwindow.ui

RESOURCES += \
    shader.qrc \
    textures.qrc \
    objects.qrc

DISTFILES += \
    compute.glsl \
    depth.vert \
    light.frag \
    light.vert \
    skybox.frag \
    skybox.vert

