TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -std=c++11

DEFINES += GLM_FORCE_RADIANS

SOURCES += \
    src/scene.cpp \
    src/main.cpp \
    src/shader.cpp \
    src/model.cpp \
    src/label.cpp \
    src/resourcebundle.cpp \
    src/labelfactory.cpp \
    src/spacebox.cpp \
    src/spritesheet.cpp \
    src/sprite.cpp \
    src/rock.cpp \
    src/drawableinstance.cpp \
    src/rockgenerator.cpp \
    src/octahedron.cpp \
    src/boundarysphere.cpp \
    src/laserbeam.cpp


HEADERS += \
    src/scene.h \
    src/shader.h \
    src/model.h \
    src/rendercontext.h \
    src/label.h \
    src/resourcebundle.h \
    src/labelfactory.h \
    src/spacebox.h \
    src/spritesheet.h \
    src/sprite.h \
    src/rock.h \
    src/drawable.h \
    src/drawableinstance.h \
    src/rockgenerator.h \
    src/lightblock.h \
    src/octahedron.h \
    src/boundarysphere.h \
    src/laserbeam.h

unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += sdl2

unix: PKGCONFIG += glew

unix: PKGCONFIG += gl

unix: PKGCONFIG += assimp
unix: PKGCONFIG += pangocairo

unix: LIBS += -lrucksack -lfreeimage


OTHER_FILES += \
    src/shaders/basic.frag \
    src/shaders/basic.vert \
    src/shaders/texture.vert \
    src/shaders/texture.frag \
    src/shaders/text.vert \
    src/shaders/text.frag \
    src/shaders/cubemap.vert \
    src/shaders/cubemap.frag \
    src/shaders/rock.vert \
    src/shaders/rock.frag \
    src/shaders/boundarysphere.vert \
    src/shaders/boundarysphere.frag \
    src/shaders/laser.vert \
    src/shaders/laser.frag

