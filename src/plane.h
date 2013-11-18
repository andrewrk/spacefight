#ifndef PLANE_H
#define PLANE_H

#include "texture.h"
#include "rendercontext.h"
#include "shader.h"
#include "shadermanager.h"

#ifndef BUFFER_OFFSET

    #define BUFFER_OFFSET(offset) ((char*)NULL + (offset))

#endif

class Plane
{
public:
    Plane(std::string texture, const RenderContext &renderContext);
    void render();


private:
    float mVertices[18];
    float mTexCoords[12];

    void load();

    GLuint mVBOid;
    GLuint mVAOid;

    Texture *mTexture;
    Shader *mShader;
    const RenderContext &mRenderContext;

    GLuint mShaderModelViewMatrix;
    GLuint mShaderNormalMatrix;
    GLuint mShaderProjectionMatrix;
    GLuint mShaderMvp;
};

#endif // PLANE_H
