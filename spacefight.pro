TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -std=c++11


SOURCES += \
    src/scene.cpp \
    src/main.cpp \
    src/shader.cpp \
    src/model.cpp \
    src/Input.cpp \
    src/shadermanager.cpp


HEADERS += \
    src/scene.h \
    src/shader.h \
    src/model.h \
    src/Input.h \
    src/shadermanager.h

unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += sdl2

unix: PKGCONFIG += glew

unix: PKGCONFIG += gl

unix: PKGCONFIG += assimp



OTHER_FILES += \
    src/shaders/basic.frag \
    src/shaders/basic.vert
