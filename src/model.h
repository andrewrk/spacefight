#ifndef MODEL_H
#define MODEL_H

#include "shader.h"
#include "rendercontext.h"

#include <memory>

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

    std::unique_ptr<Shader::UniformBlock> mLightBlock;
    std::unique_ptr<Shader::UniformBlock> mMaterialBlock;

    GLuint mShaderModelViewMatrix;
    GLuint mShaderNormalMatrix;
    GLuint mShaderProjectionMatrix;
    GLuint mShaderMvp;


    std::vector<GLuint> mVertexPositionBuffers;
    std::vector<GLuint> mVertexNormalBuffers;
    std::vector<GLuint> mVertexArrays;
    std::vector<GLuint> mVertexIndexBuffers;
    std::vector<unsigned int> mIndexCounts;

    const RenderContext &mRenderContext;

    void importNode(aiNode *node);
};

#endif // MODEL_H
