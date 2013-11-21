#include "skybox.h"

Skybox::Skybox(const std::string folder,
               const std::string front,
               const std::string back,
               const std::string top,
               const std::string bottom,
               const std::string left,
               const std::string right) :
    mFolder(folder),
    mFrontPath(front),
    mBackPath(back),
    mTopPath(top),
    mBottomPath(bottom),
    mLeftPath(left),
    mRightPath(right),
    mTextures(0),
    mVAOid(0),
    mVBOid(0),
    mSizeTextureBytes(72 * sizeof(float)),
    mSizeVerticeBytes(108 * sizeof(float))
{
    loadTextures();
    mShader = ShaderManager::getShader("texture");

    float size = 70.0f;

    float vertices[] = {-size, -size, -size,   size, -size, -size,   size, size, -size,     // Front
                        -size, -size, -size,   -size, size, -size,   size, size, -size,     // Front

                        size, -size, size,   size, -size, -size,   size, size, -size,       // Right
                        size, -size, size,   size, size, size,   size, size, -size,         // Right

                        -size, -size, size,   size, -size, size,   size, -size, -size,      // Bottom
                        -size, -size, size,   -size, -size, -size,   size, -size, -size,    // Bottom

                        -size, -size, size,   size, -size, size,   size, size, size,        // Back
                        -size, -size, size,   -size, size, size,   size, size, size,        // Back

                        -size, -size, -size,   -size, -size, size,   -size, size, size,     // Left
                        -size, -size, -size,   -size, size, -size,   -size, size, size,     // Left

                        -size, size, size,   size, size, size,   size, size, -size,         // Top
                        -size, size, size,   -size, size, -size,   size, size, -size};      // Top



    for(int i(0); i < 108; i++)
    {
        mVertices[i] = vertices[i];
    }

    float textureCoords[] = { 0,0,  1,0,   1,1,
                              0,0,  0,1,   1,1,

                              0,0,  1,0,   1,1,
                              0,0,  0,1,   1,1,

                              0,0,  1,0,   1,1,
                              0,0,  0,1,   1,1,

                              0,0,  1,0,   1,1,
                              0,0,  0,1,   1,1,

                              0,0,  1,0,   1,1,
                              0,0,  0,1,   1,1,

                              0,0,  1,0,   1,1,
                              0,0,  0,1,   1,1 };


        for(int i(0); i < 72; i++)
        {
            mTexCoords[i] = textureCoords[i];
        }

    loadBuffers();

    mShaderMvp = mShader->uniformId("MVP");

}
Skybox::~Skybox()
{
    glDeleteBuffers(1, &mVBOid);
    glDeleteVertexArrays(1, &mVAOid);
}

void Skybox::loadTextures()
{
    mTextures.push_back(new Texture(mFolder + "/" + mFrontPath));
    mTextures.push_back(new Texture(mFolder + "/" + mRightPath));
    mTextures.push_back(new Texture(mFolder + "/" + mBottomPath));
    mTextures.push_back(new Texture(mFolder + "/" + mBackPath));
    mTextures.push_back(new Texture(mFolder + "/" + mLeftPath));
    mTextures.push_back(new Texture(mFolder + "/" + mTopPath));


}


void Skybox::draw(const RenderContext &renderContext)
{
//glCullFace(GL_CW);
    mShader->bind();

    glDisable(GL_CULL_FACE);
    glUniformMatrix4fv(mShaderMvp, 1, GL_FALSE, &renderContext.mvp[0][0]);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(0);

       glBindVertexArray(mVAOid);
       for(unsigned int i(0); i < mTextures.size(); i++)
       {
           mTextures[i]->bind();
           glDrawArrays(GL_TRIANGLES, i*6, 6);
           glBindTexture(GL_TEXTURE_2D, 0);
       }
       glBindVertexArray(0);
    //glCullFace(GL_CCW);
       glEnable(GL_CULL_FACE);
    glDepthMask(1);
    glEnable(GL_DEPTH_TEST);

    mShader->unbind();

}


void Skybox::loadBuffers()
{
    if(glIsBuffer(mVBOid) == GL_TRUE)
        glDeleteBuffers(1, &mVBOid);

    glGenBuffers(1, &mVBOid);

    if(glIsVertexArray(mVAOid) == GL_TRUE)
        glDeleteVertexArrays(1, &mVAOid);

    glGenVertexArrays(1, &mVAOid);

    GLint vertexPositionIndex = mShader->attribLocation("VertexPosition");
    GLint vertexTextureIndex = mShader->attribLocation("TextureCoords");

    glBindBuffer(GL_ARRAY_BUFFER, mVBOid);
    {
        glBufferData(GL_ARRAY_BUFFER, mSizeVerticeBytes + mSizeTextureBytes, 0, GL_STATIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, mSizeVerticeBytes, mVertices);
        glBufferSubData(GL_ARRAY_BUFFER, mSizeVerticeBytes, mSizeTextureBytes, mTexCoords);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);


    glBindVertexArray(mVAOid);
    {
        glBindBuffer(GL_ARRAY_BUFFER, mVBOid);
        {
            glVertexAttribPointer(vertexPositionIndex, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
            glEnableVertexAttribArray(vertexPositionIndex);

            glVertexAttribPointer(vertexTextureIndex, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(mSizeVerticeBytes));
            glEnableVertexAttribArray(vertexTextureIndex);
        }
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    glBindVertexArray(0);


}

