#include "laserbeam.h"

#include "resourcebundle.h"
#include <iostream>

static GLfloat points[] = {
    -1, -1, 0,
    -1,  1, 0,
     1, -1, 0,
     1,  1, 0,
};

LaserBeam::LaserBeam(ResourceBundle *bundle)
{
    mShader = bundle->getShader("laser");
    mAttribVertexPosition = mShader->attribLocation("VertexPosition");
    mUniformBeamColor = mShader->uniformId("BeamColor");
    mUniformMvp = mShader->uniformId("MVP");



    glGenVertexArrays(1, &mVertexArray);
    glBindVertexArray(mVertexArray);

    glGenBuffers(1, &mVertexBuffer);

    if (mAttribVertexPosition == -1) {
        std::cerr << "warning: laser shader discarding vertex buffer data\n";
    } else {
        glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, 4 * 3 * sizeof(GLfloat), points, GL_STATIC_DRAW);
        glEnableVertexAttribArray(mAttribVertexPosition);
        glVertexAttribPointer(mAttribVertexPosition, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    }

    GLenum err = glGetError();
    assert(err == GL_NO_ERROR);
}

LaserBeam::~LaserBeam()
{
}

void LaserBeam::draw(const RenderContext &renderContext) const
{
    mShader->bind();

    mShader->setUniform(mUniformMvp, renderContext.mvp);
    mShader->setUniform(mUniformBeamColor, glm::vec3(0.75f, 0.30f, 0.95f));

    glBindVertexArray(mVertexArray);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

}
