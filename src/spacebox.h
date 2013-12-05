#ifndef SPACEBOX_H
#define SPACEBOX_H

#include "shader.h"
#include "rendercontext.h"

class ResourceBundle;

class SpaceBox
{
public:
    SpaceBox(ResourceBundle *bundle);
    ~SpaceBox();

    void generate();
    void draw(const RenderContext &renderContext);

private:
    Shader *mCubeMapShader;
    GLint mAttribVertexPosition;
    GLint mUniformVPMatrix;

    GLuint mCubeVbo = 0;
    GLuint mVao = 0;
    GLuint mTexture = 0;

    void cleanup();
};

#endif // SPACEBOX_H
