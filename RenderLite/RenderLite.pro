QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
#CONFIG += resources_big

#QMAKE_LFLAGS += -WI,--stack,32000000


# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Mesh.cpp \
    Model.cpp \
    camera.cpp \
    glwidget.cpp \
    lightData.cpp \
    main.cpp \
    mainwindow.cpp \
    matrix.cpp \
    scene.cpp \
    triangle.cpp

HEADERS += \
    Bounds3.h \
    Global.h \
    Intersection.h \
    Mesh.h \
    Model.h \
    Object.h \
    Objectmodel.h \
    Texture.h \
    Vertex.h \
    camera.h \
    glwidget.h \
    lightData.h \
    mainwindow.h \
    ray.h \
    scene.h \
    triangle.h

FORMS += \
    mainwindow.ui

DISTFILES += \
    #include/assimp/config.h.in

RESOURCES += \
    shader.qrc
    win32: LIBS += -L$$PWD/../ -llibassimp.dll

INCLUDEPATH += $$PWD/include
DEPENDPATH += $$PWD/include
