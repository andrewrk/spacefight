#include "spacebox.h"
#include <cassert>
#include <iostream>



Spacebox::Spacebox(const std::string &filename)
{
    Assimp::Importer importer;

    const aiScene *scene = importer.ReadFile(filename,
          aiProcess_Triangulate);

    assert(scene);

    mShader = ShaderManager::getShader("texture");

    mShaderMvp = mShader->uniformId("MVP");

    mVertexPositionBuffers.resize(scene->mNumMeshes);
    mVertexArrays.resize(mVertexPositionBuffers.size());
    mIndexCounts.resize(mVertexPositionBuffers.size());
    mVertexIndexBuffers.resize(mVertexPositionBuffers.size());


    std::cout << "Number of materials in scene: " << scene->mNumMaterials << std::endl;

    if(scene->HasMaterials())
    {
        int texCount(0);
        for(unsigned int i(0); i < scene->mNumMaterials; i++)
        {
            const aiMaterial* material = scene->mMaterials[i];
            assert(material);
            aiString texturePath;
            std::string texPathStdString = "";

            unsigned int numTextures = material->GetTextureCount(aiTextureType_DIFFUSE);
            std::cout << "Number of textures in material: " << std::cout << numTextures <<std::endl;

            if(material->GetTextureCount(aiTextureType_DIFFUSE) > 0 && material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == AI_SUCCESS)
            {
                texPathStdString = (std::string)(texturePath.C_Str());
                Texture tex("assets/" + texPathStdString);
                mTextures.push_back(tex);
                mTextureIds.push_back(tex.getID());
                std::cout << "Found texture with id: " << tex.getID() << " and texture path: " << texturePath.C_Str() << std::endl;
                texCount++;
            }

        }
        mTextureCoordsBuffers.resize(texCount);
        std::cout << "Size of mTextureCoordsBuffers: " << mTextureCoordsBuffers.size() << std::endl;
    }

    glGenBuffers(mVertexPositionBuffers.size(), &mVertexPositionBuffers[0]);
    glGenBuffers(mTextureCoordsBuffers.size(), &mTextureCoordsBuffers[0]);
    glGenBuffers(mVertexIndexBuffers.size(), &mVertexIndexBuffers[0]);
    glGenVertexArrays(mVertexArrays.size(), &mVertexArrays[0]);

    GLint vertexPositionIndex = mShader->attribLocation("VertexPosition");
    GLint textureIndex = mShader->attribLocation("TextureCoords");

    for(unsigned int i(0); i < mVertexPositionBuffers.size(); i++)
    {
        aiMesh *mesh = scene->mMeshes[i];
        std::vector<GLuint> indexes;
        for(unsigned int faceIndex = 0; faceIndex < mesh->mNumFaces; faceIndex++)
        {
            aiFace *face = &mesh->mFaces[faceIndex];
            for(unsigned int indexIndex = 0; indexIndex < face->mNumIndices; indexIndex++)
            {
                indexes.push_back(face->mIndices[indexIndex]);
            }
        }
        mIndexCounts[i] = indexes.size();

        GLuint vertexArrayIndex = mVertexArrays[i];
        glBindVertexArray(vertexArrayIndex);

            GLuint indexBufferId = mVertexIndexBuffers[i];
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexes.size() * sizeof(GLuint), &indexes[0], GL_STATIC_DRAW);


            if(vertexPositionIndex != -1)
            {
                GLuint positionBufferIndex = mVertexPositionBuffers[i];
                glBindBuffer(GL_ARRAY_BUFFER, positionBufferIndex);
                glBufferData(GL_ARRAY_BUFFER, mesh->mNumVertices * 3 * sizeof(GLfloat), mesh->mVertices, GL_STATIC_DRAW);

                glEnableVertexAttribArray(vertexPositionIndex);
                glVertexAttribPointer(vertexPositionIndex, 3, GL_FLOAT, GL_FALSE, 0, NULL);
            }
            if(textureIndex != -1)
            {
                GLuint textureBufferIndex = mTextureCoordsBuffers[i];
                glBindBuffer(GL_ARRAY_BUFFER, textureBufferIndex);
                //int numTexCoords = (int)(mesh->mNumUVComponents);
                glBufferData(GL_ARRAY_BUFFER, 72 * sizeof(GLfloat), mesh->mTextureCoords, GL_STATIC_DRAW);

                std::cout << "Number of tex coords: " << mesh->mTextureCoords << std::endl;

                glEnableVertexAttribArray(textureBufferIndex);
                glVertexAttribPointer(textureBufferIndex, 2, GL_FLOAT, GL_FALSE, 0, NULL);
            }

        glBindVertexArray(0);

        GLenum err = glGetError();
        assert(err == GL_NO_ERROR);
    }

}

void Spacebox::draw(const RenderContext &renderContext)
{
    mShader->bind();

    mShader->setUniform(mShaderMvp, renderContext.mvp);

    glDepthMask(0);

    for(unsigned int i(0); i < mVertexArrays.size(); i++)
    {
        glBindVertexArray(mVertexArrays[i]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mVertexIndexBuffers[i]);
        glBindTexture(GL_TEXTURE_2D, mTextures[i].getID());
        glDrawElements(GL_TRIANGLES, mIndexCounts[i], GL_UNSIGNED_INT, NULL);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    glDepthMask(1);

    mShader->unbind();

}
