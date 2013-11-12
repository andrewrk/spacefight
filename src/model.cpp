#include "model.h"
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

    mVertexBuffers.resize(scene->mNumMeshes);
    mVertexCounts.resize(mVertexBuffers.size());
    for (unsigned int i = 0; i < mVertexBuffers.size(); i += 1) {
        GLuint vertexBufferIndex;
        glGenBuffers(1, &vertexBufferIndex);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferIndex);
        aiMesh *mesh = scene->mMeshes[i];
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glBufferData(GL_ARRAY_BUFFER, mesh->mNumVertices, mesh->mVertices, GL_STATIC_DRAW);
        mVertexCounts[i] = mesh->mNumVertices;
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        mVertexBuffers[i] = vertexBufferIndex;
        glEnableVertexAttribArray(0);
    }
}

void Model::draw()
{

    for (unsigned int i = 0; i < mVertexBuffers.size(); i += 1) {
        GLuint vertexBuffer = mVertexBuffers[i];
        unsigned int vertexCount = mVertexCounts[i];
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glDrawArrays(GL_TRIANGLES, 0, vertexCount);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}
