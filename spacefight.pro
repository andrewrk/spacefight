TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -std=c++11


SOURCES += \
    src/scene.cpp \
    src/main.cpp \
    src/shader.cpp


HEADERS += \
    src/scene.h \
    src/shader.h

unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += sdl2

unix: PKGCONFIG += glew

unix: PKGCONFIG += gl

OTHER_FILES += \
    src/shaders/basic.frag \
    src/shaders/basic.vert
