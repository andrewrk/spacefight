TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -std=c++11


SOURCES += \
    src/scene.cpp \
    src/main.cpp


HEADERS += \
    src/scene.h

unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += sdl2
