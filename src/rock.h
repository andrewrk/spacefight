#ifndef ROCK_H
#define ROCK_H

#include "GL/glew.h"

#include "shader.h"
#include "rendercontext.h"

#include <memory>

class ResourceBundle;

class Rock
{
public:
    Rock(ResourceBundle *bundle);
    ~Rock();

    void generate();

    void draw(const RenderContext &renderContext);

private:
    GLuint vertexArrayObject = 0;
    GLuint vertexPositionBuffer = 0;
    GLuint vertexNormalBuffer = 0;
    GLuint vertexIndexBuffer = 0;

    Shader *mShader;

    GLint attribPositionIndex;
    GLint attribNormalIndex;

    std::unique_ptr<Shader::UniformBlock> mLightBlock;
    std::unique_ptr<Shader::UniformBlock> mMaterialBlock;

    GLuint mShaderModelViewMatrix;
    GLuint mShaderNormalMatrix;
    GLuint mShaderProjectionMatrix;
    GLuint mShaderMvp;

    int elementCount;

    void cleanup();
};

#endif // ROCK_H
