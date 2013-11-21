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
    src/shadermanager.cpp \
    src/texture.cpp \
    src/skybox.cpp \
    src/label.cpp \
    src/spacebox.cpp


HEADERS += \
    src/scene.h \
    src/shader.h \
    src/model.h \
    src/shadermanager.h \
    src/rendercontext.h \
    src/texture.h \
    src/skybox.h \
    src/label.h \
    src/spacebox.h

unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += sdl2

unix: PKGCONFIG += glew

unix: PKGCONFIG += gl

unix: PKGCONFIG += assimp
unix: PKGCONFIG += pangocairo

unix: LIBS += -lSDL2_image


OTHER_FILES += \
    src/shaders/basic.frag \
    src/shaders/basic.vert \
    src/shaders/texture.vert \
    src/shaders/texture.frag \
    src/shaders/text.vert \
    src/shaders/text.frag

