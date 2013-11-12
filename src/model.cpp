#include "model.h"
#include "shadermanager.h"
#include <cassert>

#include <iostream>

Model::Model(const std::string &filename)
{
    Assimp::Importer importer;

    const aiScene *scene = importer.ReadFile(filename,
        aiProcess_Triangulate|
        aiProcess_CalcTangentSpace|
        aiProcess_JoinIdenticalVertices|
        aiProcess_SortByPType|
        aiProcess_GenNormals);

    assert(scene);

    mShader = ShaderManager::getShader("basic");
    mShader->bindAttribLocation(0, "VertexPosition");

    mShaderMvp = mShader->uniformId("MVP");

    mVertexBuffers.resize(scene->mNumMeshes);
    mVertexCounts.resize(mVertexBuffers.size());
    mVertexArrays.resize(mVertexBuffers.size());
    glGenBuffers(mVertexBuffers.size(), &mVertexBuffers[0]);
    glGenVertexArrays(mVertexArrays.size(), &mVertexArrays[0]);
    for (unsigned int i = 0; i < mVertexBuffers.size(); i += 1) {
        aiMesh *mesh = scene->mMeshes[i];
        mVertexCounts[i] = mesh->mNumVertices;

        GLuint vertexBufferIndex = mVertexBuffers[i];
        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferIndex);
        glBufferData(GL_ARRAY_BUFFER, mesh->mNumVertices * 3 * sizeof(GL_FLOAT), mesh->mVertices, GL_STATIC_DRAW);

        GLuint vertexArrayIndex = mVertexArrays[i];
        glBindVertexArray(vertexArrayIndex);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferIndex);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    }
}

void Model::draw(const glm::mat4 &mvp)
{
    mShader->bind();
    glUniformMatrix4fv(mShaderMvp, 1, GL_FALSE, &mvp[0][0]);

    for (unsigned int i = 0; i < mVertexArrays.size(); i += 1) {
        GLuint vaoHandle = mVertexArrays[i];
        unsigned int vertexCount = mVertexCounts[i];
        glBindVertexArray(vaoHandle);
        glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    }

    mShader->unbind();
}
