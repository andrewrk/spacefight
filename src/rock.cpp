#include "rock.h"

#include "resourcebundle.h"
#include "rockgenerator.h"
#include "lightblock.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <vector>
#include <iostream>


Rock::Rock()
{
}

Rock::~Rock()
{

    cleanup();
}


void Rock::draw(const RenderContext &renderContext) const
{
    generator->mShader->bind();
    generator->mShader->setUniform(generator->mShaderMvp, renderContext.mvp);
    generator->mShader->setUniform(generator->mShaderModelViewMatrix, renderContext.modelView);
    generator->mShader->setUniform(generator->mShaderProjectionMatrix, renderContext.projection);
    generator->mShader->setUniform(generator->mShaderNormalMatrix, renderContext.normal);
    generator->mShader->setUniform(generator->texUniformId, 0);

    generator->mLightBlock->set(LIGHT_LA, renderContext.lightIntensityAmbient);
    generator->mLightBlock->set(LIGHT_LD, renderContext.lightIntensityDiffuse);
    generator->mLightBlock->set(LIGHT_LS, renderContext.lightIntensitySpecular);
    generator->mLightBlock->set(LIGHT_DIRECTION, renderContext.lightDirectionEye);
    generator->mLightBlock->update();

    generator->mMaterialBlock->set(MATERIAL_KA, renderContext.materialReflectivityAmbient);
    generator->mMaterialBlock->set(MATERIAL_KD, renderContext.materialReflectivityDiffuse);
    generator->mMaterialBlock->set(MATERIAL_KS, renderContext.materialReflectivitySpecular);
    generator->mMaterialBlock->set(MATERIAL_SHININESS, renderContext.materialSpecularShininess);
    generator->mMaterialBlock->update();

    glBindVertexArray(vertexArrayObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexIndexBuffer);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, generator->mTexture);
    glDrawElements(GL_TRIANGLE_STRIP, elementCount, GL_UNSIGNED_INT, NULL);
}

void Rock::cleanup()
{
    if (vertexPositionBuffer) {
        glDeleteBuffers(1, &vertexPositionBuffer);
        vertexPositionBuffer = 0;
    }
    if (vertexNormalBuffer) {
        glDeleteBuffers(1, &vertexNormalBuffer);
        vertexNormalBuffer = 0;
    }
    if (vertexIndexBuffer) {
        glDeleteBuffers(1, &vertexIndexBuffer);
        vertexIndexBuffer = 0;
    }
    if (texCoordBuffer) {
        glDeleteBuffers(1, &texCoordBuffer);
        texCoordBuffer = 0;
    }
    if (vertexArrayObject) {
        glDeleteVertexArrays(1, &vertexArrayObject);
        vertexArrayObject = 0;
    }
}
