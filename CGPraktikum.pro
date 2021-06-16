QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    model.cpp \
    myglwidget.cpp \
    skybox.cpp

HEADERS += \
    mainwindow.h \
    model.h \
    modelloader/modelloader.h \
    myglwidget.h \
    skybox.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    back.jpg \
    backpack.obj \
    bottom.jpg \
    front.jpg \
    gimbal-smooth.obj \
    gimbal.obj \
    gimbal_wood.jpg \
    left.jpg \
    light.frag \
    light.vert \
    right.jpg \
    sample.frag \
    sample.vert \
    sample_texture.jpg \
    skybox.frag \
    skybox.vert \
    sphere.obj \
    star.glb \
    top.jpg

RESOURCES += \
    ressource.qrc

win32 {
    contains(QT_ARCH, i386) {
        # for 32 bit windows applications
        message(Target: win32)
        LIBS += -L../assimp-mingw32-4.1.0/bin -lassimp
        INCLUDEPATH += ../assimp-mingw32-4.1.0/include
    } else {
        # for 64 bit windows applications
        message(Target: x64)
        LIBS += -L../assimp-mingw_w64-5.01/bin -lassimp
        INCLUDEPATH += ../assimp-mingw_w64-5.01/include
    }
} unix {
    message(Target: unix)
    CONFIG += link_pkgconfig
    PKGCONFIG += assimp
 }
