#ifndef MODEL_H
#define MODEL_H

#include "shader.h"
#include "rendercontext.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glm/glm.hpp>

#include <GL/glew.h>

class Model
{
public:
    Model(const std::string &filename, const RenderContext &renderContext);

    void draw();

private:
    Shader *mShader;
    GLuint mShaderMvp;
    GLuint mShaderLightPosition;
    GLuint mShaderDiffuseReflectivity;
    GLuint mShaderLightSourceIntensity;
    GLuint mShaderModelViewMatrix;
    GLuint mShaderNormalMatrix;
    GLuint mShaderProjectionMatrix;


    std::vector<GLuint> mVertexPositionBuffers;
    std::vector<GLuint> mVertexNormalBuffers;
    std::vector<GLuint> mVertexArrays;
    std::vector<GLuint> mVertexIndexBuffers;
    std::vector<unsigned int> mIndexCounts;

    const RenderContext &mRenderContext;

    void importNode(aiNode *node);
};

#endif // MODEL_H
