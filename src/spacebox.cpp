#include "spacebox.h"

#include "resourcebundle.h"

#include <iostream>

static GLfloat points[] = {
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
};

SpaceBox::SpaceBox(ResourceBundle *bundle)
{
    mCubeMapShader = bundle->getShader("cubemap");
    mAttribVertexPosition = mCubeMapShader->attribLocation("VertexPosition");
    mUniformVPMatrix = mCubeMapShader->uniformId("MVP");

    mBundle = bundle;
}

SpaceBox::~SpaceBox()
{
    cleanup();
}

void SpaceBox::draw(const RenderContext &renderContext)
{
    mCubeMapShader->bind();

    mCubeMapShader->setUniform(mUniformVPMatrix, renderContext.mvp);

    glBindVertexArray(mVao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, mTexture);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 36);

    mCubeMapShader->unbind();
}

void SpaceBox::cleanup()
{

    if (mCubeVbo) {
        glDeleteBuffers(1, &mCubeVbo);
        mCubeVbo = 0;
    }

    if (mVao) {
        glDeleteVertexArrays(1, &mVao);
        mVao = 0;
    }

    if (mTexture) {
        glDeleteTextures(1, &mTexture);
        mTexture = 0;
    }
}

void SpaceBox::generate()
{
    cleanup();

    glGenVertexArrays(1, &mVao);
    glBindVertexArray(mVao);

    if (mAttribVertexPosition != -1) {
        glGenBuffers(1, &mCubeVbo);
        glBindBuffer(GL_ARRAY_BUFFER, mCubeVbo);
        glBufferData(GL_ARRAY_BUFFER, 3 * 36 * sizeof(GLfloat), &points, GL_STATIC_DRAW);

        glEnableVertexAttribArray(mAttribVertexPosition);
        glVertexAttribPointer(mAttribVertexPosition, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    } else {
        std::cerr << "warning: cubemap shader is ignoring VertexPosition data";
    }

    glGenTextures(1, &mTexture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, mTexture);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

//    int size = 512;
//    std::vector<unsigned char> starPic;
//    int oneFaceSize = size * size * 4;
//    starPic.resize(oneFaceSize * 6);
//    for (unsigned int i = 0; i < starPic.size(); i += 4) {
//        unsigned char isStar = (rand() % 200) == 0;
//        unsigned char starIntensity = (rand() % 256);
//        unsigned char pixelColor = isStar * starIntensity;
//        starPic[i+0] = pixelColor;
//        starPic[i+1] = pixelColor;
//        starPic[i+2] = pixelColor;
//        starPic[i+3] = 255;
//    }

//    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA8, size, size, 0, GL_BGRA, GL_UNSIGNED_BYTE, &starPic[oneFaceSize*0]);
//    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA8, size, size, 0, GL_BGRA, GL_UNSIGNED_BYTE, &starPic[oneFaceSize*1]);
//    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA8, size, size, 0, GL_BGRA, GL_UNSIGNED_BYTE, &starPic[oneFaceSize*2]);
//    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA8, size, size, 0, GL_BGRA, GL_UNSIGNED_BYTE, &starPic[oneFaceSize*3]);
//    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA8, size, size, 0, GL_BGRA, GL_UNSIGNED_BYTE, &starPic[oneFaceSize*4]);
//    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA8, size, size, 0, GL_BGRA, GL_UNSIGNED_BYTE, &starPic[oneFaceSize*5]);

    ResourceBundle::Image image;
    image.doPixelStoreAlignment();

    mBundle->getImage("skyboxright.png", image);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA8, image.width(), image.height(), 0, image.format(), image.type(), image.pixels());

    mBundle->getImage("skyboxleft.png", image);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA8, image.width(), image.height(), 0, image.format(), image.type(), image.pixels());

    mBundle->getImage("skyboxbottom.png", image);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA8, image.width(), image.height(), 0, image.format(), image.type(), image.pixels());

    mBundle->getImage("skyboxtop.png", image);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA8, image.width(), image.height(), 0, image.format(), image.type(), image.pixels());

    mBundle->getImage("skyboxfront.png", image);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA8, image.width(), image.height(), 0, image.format(), image.type(), image.pixels());

    mBundle->getImage("skyboxback.png", image);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA8, image.width(), image.height(), 0, image.format(), image.type(), image.pixels());

    image.resetPixelStoreAlignment();
}
