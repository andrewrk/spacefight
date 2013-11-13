#include "model.h"
#include "shadermanager.h"
#include <cassert>

#include <iostream>

Model::Model(const std::string &filename, const RenderContext &renderContext) :
    mRenderContext(renderContext)
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

    mShaderMvp = mShader->uniformId("MVP");
    mShaderLightPosition = mShader->uniformId("LightPosition");
    mShaderDiffuseReflectivity = mShader->uniformId("DiffuseReflectivity");
    mShaderLightSourceIntensity = mShader->uniformId("LightSourceIntensity");
    mShaderModelViewMatrix = mShader->uniformId("ModelViewMatrix");
    mShaderNormalMatrix = mShader->uniformId("NormalMatrix");
    mShaderProjectionMatrix = mShader->uniformId("ProjectionMatrix");

    mVertexPositionBuffers.resize(scene->mNumMeshes);
    mVertexNormalBuffers.resize(mVertexPositionBuffers.size());
    mIndexCounts.resize(mVertexPositionBuffers.size());
    mVertexArrays.resize(mVertexPositionBuffers.size());
    mVertexIndexBuffers.resize(mVertexPositionBuffers.size());



    glGenBuffers(mVertexPositionBuffers.size(), &mVertexPositionBuffers[0]);
    glGenBuffers(mVertexNormalBuffers.size(), &mVertexNormalBuffers[0]);
    glGenBuffers(mVertexIndexBuffers.size(), &mVertexIndexBuffers[0]);
    glGenVertexArrays(mVertexArrays.size(), &mVertexArrays[0]);

    GLint vertexPositionIndex = mShader->attribLocation("VertexPosition");
    GLint vertexNormalIndex = mShader->attribLocation("VertexNormal");
    for (unsigned int i = 0; i < mVertexPositionBuffers.size(); i += 1) {
        aiMesh *mesh = scene->mMeshes[i];
        std::vector<GLuint> indexes;
        for (unsigned int faceIndex = 0; faceIndex < mesh->mNumFaces; faceIndex += 1) {
            aiFace *face = &mesh->mFaces[faceIndex];
            for (unsigned int indexIndex = 0; indexIndex < face->mNumIndices; indexIndex += 1) {
                indexes.push_back(face->mIndices[indexIndex]);
            }
        }
        mIndexCounts[i] = indexes.size();


        GLuint vertexArrayIndex = mVertexArrays[i];
        glBindVertexArray(vertexArrayIndex);


        GLuint indexBufferId = mVertexIndexBuffers[i];
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexes.size() * sizeof(GLuint), &indexes[0], GL_STATIC_DRAW);

        if (vertexPositionIndex != -1) {
            GLuint positionBufferIndex = mVertexPositionBuffers[i];
            glBindBuffer(GL_ARRAY_BUFFER, positionBufferIndex);
            glBufferData(GL_ARRAY_BUFFER, mesh->mNumVertices * 3 * sizeof(GLfloat), mesh->mVertices, GL_STATIC_DRAW);

            glEnableVertexAttribArray(vertexPositionIndex);
            glBindBuffer(GL_ARRAY_BUFFER, positionBufferIndex);
            glVertexAttribPointer(vertexPositionIndex, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        }

        if (vertexNormalIndex != -1) {
            GLuint normalBufferIndex = mVertexNormalBuffers[i];
            glBindBuffer(GL_ARRAY_BUFFER, normalBufferIndex);
            glBufferData(GL_ARRAY_BUFFER, mesh->mNumVertices * 3 * sizeof(GLfloat), mesh->mNormals, GL_STATIC_DRAW);

            glEnableVertexAttribArray(vertexNormalIndex);
            glBindBuffer(GL_ARRAY_BUFFER, normalBufferIndex);
            glVertexAttribPointer(vertexNormalIndex, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        }
    }

    GLenum err = glGetError();
    assert(err == GL_NO_ERROR);
}

void Model::draw()
{
    mShader->bind();
    glUniformMatrix4fv(mShaderMvp, 1, GL_FALSE, &mRenderContext.mvp[0][0]);
    glUniformMatrix4fv(mShaderModelViewMatrix, 1, GL_FALSE, &mRenderContext.modelView[0][0]);
    glUniformMatrix4fv(mShaderProjectionMatrix, 1, GL_FALSE, &mRenderContext.projection[0][0]);

    glUniform4fv(mShaderLightPosition, 1, &mRenderContext.lightPosition[0]);

    glUniform3fv(mShaderDiffuseReflectivity, 1, &mRenderContext.diffuseReflectivity[0]);
    glUniform3fv(mShaderLightSourceIntensity, 1, &mRenderContext.lightSourceIntensity[0]);

    glUniformMatrix3fv(mShaderNormalMatrix, 1, GL_FALSE, &mRenderContext.normal[0][0]);


    for (unsigned int i = 0; i < mVertexArrays.size(); i += 1) {
        GLuint vaoHandle = mVertexArrays[i];
        GLuint indexBufferId = mVertexIndexBuffers[i];
        unsigned int indexCount = mIndexCounts[i];
        glBindVertexArray(vaoHandle);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, NULL);
    }

    mShader->unbind();
}
