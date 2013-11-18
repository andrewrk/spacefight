#include "plane.h"

Plane::Plane(std::string texture, const RenderContext &renderContext) : mRenderContext(renderContext)
{
    float texRepY(1.0f);
    float texRepX(1.0f);
    float size(4);
    mTexture = new Texture(texture);
    mTexture->load();

    float verts[] = { -size, 0, size,   -size, 0, -size,    size, 0, size,
                          size, 0, size,    -size, 0, -size,    size, 0, -size };

    float tex[] = { 0, texRepY,    0, 0,    texRepX, texRepY,
                    texRepX, texRepY,   0, 0,   texRepX, 0 } ;

    for(int i(0); i < 18; i++)
        mVertices[i] = verts[i];

    for(int i(0); i < 12; i++)
        mTexCoords[i] = tex[i];

    mShader = ShaderManager::getShader("texture");

    mShaderModelViewMatrix = mShader->uniformId("ModelViewMatrix");
    mShaderProjectionMatrix = mShader->uniformId("ProjectionMatrix");
    mShaderMvp = mShader->uniformId("MVP");

    load();



}

void Plane::render()
{

    mShader->bind();

        glUniformMatrix4fv(mShaderMvp, 1, GL_FALSE, &mRenderContext.mvp[0][0]);
        glUniformMatrix4fv(mShaderModelViewMatrix, 1, GL_FALSE, &mRenderContext.modelView[0][0]);
        glUniformMatrix4fv(mShaderProjectionMatrix, 1, GL_FALSE, &mRenderContext.projection[0][0]);
        //glUniformMatrix3fv(mShaderNormalMatrix, 1, GL_FALSE, &mRenderContext.normal[0][0]);

        glBindVertexArray(mVAOid);

            glBindTexture(GL_TEXTURE_2D, mTexture->getID());

                glDrawArrays(GL_TRIANGLES, 0, 6);

            glBindTexture(GL_TEXTURE_2D, 0);

        glBindVertexArray(0);

    mShader->unbind();
}

void Plane::load()
{
    if(glIsBuffer(mVBOid) == GL_TRUE)
        glDeleteBuffers(1, &mVBOid);
    glGenBuffers(1, &mVBOid);

    if(glIsVertexArray(mVAOid) == GL_TRUE)
        glDeleteVertexArrays(1, &mVAOid);
    glGenVertexArrays(1, &mVAOid);

    GLint vertexPositionIndex = mShader->attribLocation("VertexPosition");
    GLint vertexTextureIndex = mShader->attribLocation("TextureCoords");

    std::cout << "Position attribute: " << vertexPositionIndex << " and texture attrib: " << vertexTextureIndex << std::endl;

    glBindBuffer(GL_ARRAY_BUFFER, mVBOid);
    {
        float sizeVertices = 18 * sizeof(float);
        float sizeTex = 12 * sizeof(float);

        glBufferData(GL_ARRAY_BUFFER, sizeVertices + sizeTex, 0, GL_STATIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeVertices, mVertices);
        glBufferSubData(GL_ARRAY_BUFFER, sizeVertices, sizeTex, mTexCoords);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(mVAOid);
    {
        glBindBuffer(GL_ARRAY_BUFFER, mVBOid);
        {
            glVertexAttribPointer(vertexPositionIndex, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
            glEnableVertexAttribArray(vertexPositionIndex);

            glVertexAttribPointer(vertexTextureIndex, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(18*sizeof(float)));
            glEnableVertexAttribArray(vertexTextureIndex);
        }
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    glBindVertexArray(0);
}
