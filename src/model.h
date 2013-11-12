#ifndef MODEL_H
#define MODEL_H

#include "shader.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glm/glm.hpp>

#include <GL/glew.h>

class Model
{
public:
    Model(const std::string &filename);

    void draw(const glm::mat4 &mvp);

private:
    Shader *mShader;
    GLuint mShaderMvp;

    std::vector<GLuint> mVertexBuffers;
    std::vector<GLuint> mVertexArrays;
    std::vector<unsigned int> mVertexCounts;

    void importNode(aiNode *node);
};

#endif // MODEL_H
