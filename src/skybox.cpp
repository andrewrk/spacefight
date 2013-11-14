#include "skybox.h"

Skybox::Skybox(const std::string folder, const std::string front, const std::string back, const std::string top, const std::string bottom, const std::string left, const std::string right)
    :mFolder(folder), mFrontPath(front), mBackPath(back), mTopPath(top), mBottomPath(bottom), mLeftPath(left), mRightPath(right)
{
    mTextures[0] = new Texture(mFolder + mFrontPath);
    mTextures[1] = new Texture(mFolder + mBackPath);
    mTextures[1] = new Texture(mFolder + mBackPath);
    mTextures[1] = new Texture(mFolder + mBackPath);
    mTextures[1] = new Texture(mFolder + mBackPath);


}

Skybox::~Skybox()
{
}

void Skybox::render()
{
}
