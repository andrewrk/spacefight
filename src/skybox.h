#ifndef SKYBOX_H
#define SKYBOX_H

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "texture.h"
#include "shadermanager.h"
#include "rendercontext.h"
#include "shader.h"
#include <iostream>
#include <string>
#include <vector>

#ifndef BUFFER_OFFSET

    #define BUFFER_OFFSET(offset) ((char*)NULL + (offset))

#endif

class Skybox
{
public:
    Skybox(std::string const folder,
           std::string const front,
           std::string const back,
           std::string const top,
           std::string const bottom,
           std::string const left,
           std::string const right);
    ~Skybox();
    void loadTextures();
    void loadBuffers();
    void draw(const RenderContext &renderContext);
private:
    std::string mFolder;
    std::string mFrontPath;
    std::string mBackPath;
    std::string mTopPath;
    std::string mBottomPath;
    std::string mLeftPath;
    std::string mRightPath;


    std::vector<Texture*> mTextures;
    Shader *mShader;

    GLuint mVAOid;
    GLuint mVBOid;

    float mVertices[108];
    float mTexCoords[72];
    int mSizeTextureBytes;
    int mSizeVerticeBytes;

    GLuint mShaderModelViewMatrix;
    GLuint mShaderNormalMatrix;
    GLuint mShaderProjectionMatrix;
    GLuint mShaderMvp;



};

#endif // SKYBOX_H
