#include "model.h"
#include "resourcebundle.h"
#include <cassert>
#include <iostream>

static const GLchar *LIGHT_BLOCK_FIELDS[] = {
    "Position",
    "La",
    "Ld",
    "Ls",
    NULL,
};

enum LightBlockField {
    LIGHT_POSITION,
    LIGHT_LA,
    LIGHT_LD,
    LIGHT_LS
};

static const GLchar *MATERIAL_BLOCK_FIELDS[] = {
    "Ka",
    "Kd",
    "Ks",
    "Shininess",
    NULL,
};

enum MaterialBlock {
    MATERIAL_KA,
    MATERIAL_KD,
    MATERIAL_KS,
    MATERIAL_SHININESS
};

Model::Model(ResourceBundle *bundle, const std::string &filename)
{
    Assimp::Importer importer;

    std::vector<unsigned char> modelBuffer;
    bundle->getFileBuffer(filename, modelBuffer);

    const aiScene *scene = importer.ReadFileFromMemory(&modelBuffer[0], modelBuffer.size(),
        aiProcess_Triangulate|
        aiProcess_CalcTangentSpace|
        aiProcess_JoinIdenticalVertices|
        aiProcess_SortByPType|
        aiProcess_GenSmoothNormals,
            filename.c_str());

    if (!scene) {
        std::cerr << "Error loading model: " << importer.GetErrorString() << "\n";
        exit(1);
    }

    mShader = bundle->getShader("basic");

    mLightBlock = mShader->getUniformBlock("Light", LIGHT_BLOCK_FIELDS, 0);
    mMaterialBlock = mShader->getUniformBlock("Material", MATERIAL_BLOCK_FIELDS, 1);


    mShaderModelViewMatrix = mShader->uniformId("ModelViewMatrix");
    mShaderNormalMatrix = mShader->uniformId("NormalMatrix");
    mShaderProjectionMatrix = mShader->uniformId("ProjectionMatrix");
    mShaderMvp = mShader->uniformId("MVP");

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
            glVertexAttribPointer(vertexPositionIndex, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        } else {
            std::cerr << "warning: shader ignoring vertex position data\n";
        }

        if (vertexNormalIndex != -1) {
            GLuint normalBufferIndex = mVertexNormalBuffers[i];
            glBindBuffer(GL_ARRAY_BUFFER, normalBufferIndex);
            glBufferData(GL_ARRAY_BUFFER, mesh->mNumVertices * 3 * sizeof(GLfloat), mesh->mNormals, GL_STATIC_DRAW);

            glEnableVertexAttribArray(vertexNormalIndex);
            glVertexAttribPointer(vertexNormalIndex, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        } else {
            std::cerr << "warning: shader ignoring vertex normal data\n";
        }

    }

    GLenum err = glGetError();
    assert(err == GL_NO_ERROR);
}

Model::~Model()
{
    glDeleteBuffers(mVertexPositionBuffers.size(), &mVertexPositionBuffers[0]);
    glDeleteBuffers(mVertexNormalBuffers.size(), &mVertexNormalBuffers[0]);
    glDeleteBuffers(mVertexIndexBuffers.size(), &mVertexIndexBuffers[0]);
    glDeleteVertexArrays(mVertexArrays.size(), &mVertexArrays[0]);
}

void Model::draw(const RenderContext &renderContext) const
{
    mShader->bind();
    mShader->setUniform(mShaderMvp, renderContext.mvp);
    mShader->setUniform(mShaderModelViewMatrix, renderContext.modelView);
    mShader->setUniform(mShaderProjectionMatrix, renderContext.projection);
    mShader->setUniform(mShaderNormalMatrix, renderContext.normal);

    mLightBlock->set(LIGHT_LA, renderContext.lightIntensityAmbient);
    mLightBlock->set(LIGHT_LD, renderContext.lightIntensityDiffuse);
    mLightBlock->set(LIGHT_LS, renderContext.lightIntensitySpecular);
    mLightBlock->set(LIGHT_POSITION, renderContext.lightPosition);
    mLightBlock->update();

    mMaterialBlock->set(MATERIAL_KA, renderContext.materialReflectivityAmbient);
    mMaterialBlock->set(MATERIAL_KD, renderContext.materialReflectivityDiffuse);
    mMaterialBlock->set(MATERIAL_KS, renderContext.materialReflectivitySpecular);
    mMaterialBlock->set(MATERIAL_SHININESS, renderContext.materialSpecularShininess);
    mMaterialBlock->update();

    for (unsigned int i = 0; i < mVertexArrays.size(); i += 1) {
        GLuint vaoHandle = mVertexArrays[i];
        GLuint indexBufferId = mVertexIndexBuffers[i];
        unsigned int indexCount = mIndexCounts[i];
        glBindVertexArray(vaoHandle);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, NULL);
    }
}
