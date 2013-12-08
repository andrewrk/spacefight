#include "spritesheet.h"

#include "resourcebundle.h"

SpriteSheet::SpriteSheet()
{

}

SpriteSheet::~SpriteSheet()
{
    cleanup();
}

void SpriteSheet::draw(ImageInfo *image, const RenderContext &renderContext)
{
    mShader->bind();

    mShader->setUniform(mvpUniformId, renderContext.mvp);
    mShader->setUniform(texUniformId, 0);

    glBindVertexArray(image->vertexArray);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mGlTex);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void SpriteSheet::init(ResourceBundle *bundle)
{
    mShader = bundle->getShader("texture");

    attribTexCoord = mShader->attribLocation("TexCoord");
    attribPosition = mShader->attribLocation("VertexPosition");

    texUniformId = mShader->uniformId("Tex");
    mvpUniformId = mShader->uniformId("MVP");
}

void SpriteSheet::cleanup()
{
    if (mGlTex) {
        glDeleteTextures(1, &mGlTex);
        mGlTex = 0;
    }
    for (std::map<std::string, ImageInfo>::iterator it = mInfoDict.begin(); it != mInfoDict.end(); ++it) {
        ImageInfo *info = &it->second;
        glDeleteBuffers(1, &info->texCoordBuffer);
        glDeleteBuffers(1, &info->vertexBuffer);
        glDeleteVertexArrays(1, &info->vertexArray);
    }
    mInfoDict.clear();
}
