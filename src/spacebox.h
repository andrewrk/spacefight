#ifndef SPACEBOX_H
#define SPACEBOX_H

#include "shader.h"
#include "rendercontext.h"
#include "texture.h"
#include "shadermanager.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glm/glm.hpp>
#include <GL/glew.h>

class Spacebox
{
public:
    Spacebox(const std::string &filename);

    void draw(const RenderContext &renderContext);

private:
    Shader *mShader;

    GLuint mShaderModelViewMatrix;
    GLuint mShaderProjectionMatrix;
    GLuint mShaderMvp;

    std::vector<GLuint> mVertexPositionBuffers;
    std::vector<GLuint> mTextureCoordsBuffers;
    std::vector<GLuint> mTextureIds;
    std::vector<Texture> mTextures;

    std::vector<GLuint> mVertexArrays;
    std::vector<GLuint> mVertexIndexBuffers;
    std::vector<unsigned int> mIndexCounts;

    void importNode(aiNode *node);



};

#endif // SPACEBOX_H
