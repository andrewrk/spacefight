#ifndef SKYBOX_H
#define SKYBOX_H

#include <GL/glew.h>

#include "texture.h"
#include <iostream>
#include <string>
#include <vector>

class Skybox
{
public:
    Skybox(std::string const folder, std::string const front, std::string const back, std::string const top, std::string const bottom, std::string const left, std::string const right);
    ~Skybox();
    void render();
private:
    std::string mFrontPath;
    std::string mBackPath;
    std::string mTopPath;
    std::string mBottomPath;
    std::string mLeftPath;
    std::string mRightPath;
    std::string mFolder;

    Texture* mTextures[6];

};

#endif // SKYBOX_H
