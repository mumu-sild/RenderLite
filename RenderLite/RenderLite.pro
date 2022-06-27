QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    BVH.cpp \
    Mesh.cpp \
    Model.cpp \
    QShader.cpp \
    QVBO.cpp \
    camera.cpp \
    glwidget.cpp \
    logo.cpp \
    main.cpp \
    mainwindow.cpp \
    matrix.cpp \
    vec.cpp

HEADERS += \
    BVH.h \
    Bounds3.h \
    Global.h \
    Intersection.h \
    Mesh.h \
    Model.h \
    Object.h \
    QShader.h \
    QVBO.h \
    camera.h \
    glwidget.h \
    logo.h \
    mainwindow.h \
    matrix.h \
    ray.h \
    vec.h

FORMS += \
    mainwindow.ui

DISTFILES += \
    #include/assimp/config.h.in

RESOURCES += \
    shader.qrc
    win32: LIBS += -L$$PWD/../ -llibassimp.dll

INCLUDEPATH += $$PWD/include
DEPENDPATH += $$PWD/include
