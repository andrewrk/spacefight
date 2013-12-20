#include "laserbeam.h"

#include "resourcebundle.h"
#include <glm/glm.hpp>
#include <iostream>

struct LineInfo {
    glm::vec3 posStart;
    glm::vec3 posEnd;
    float offset;
};

static GLfloat posStartData[] = {
    -1, 0, 0,
    -1, 0, 0,
    1, 0, 0,
    1, 0, 0,
};

static GLfloat posEndData[] = {
    1, 0, 0,
    1, 0, 0,
    -1, 0, 0,
    -1, 0, 0,
};

static GLfloat offsetData[] = {
    0.2f,
    -0.2f,
    -0.2f,
    0.2f,
};

LaserBeam::LaserBeam(ResourceBundle *bundle)
{
    mShader = bundle->getShader("laser");
    mAttribPosStart = mShader->attribLocation("PosStart");
    mAttribPosEnd = mShader->attribLocation("PosEnd");
    mAttribOffset = mShader->attribLocation("Offset");

    mUniformBeamColor = mShader->uniformId("BeamColor");
    mUniformMvp = mShader->uniformId("MVP");

    mShader->bind();
    mShader->setUniform(mUniformBeamColor, glm::vec3(0.63f, 0.50f, 0.98));


    glGenVertexArrays(1, &mVertexArray);
    glBindVertexArray(mVertexArray);

    glGenBuffers(1, &mPosStartBuffer);
    glGenBuffers(1, &mPosEndBuffer);
    glGenBuffers(1, &mOffsetBuffer);


    if (mAttribPosStart == -1) {
        std::cerr << "warning: laser shader discarding pos start data\n";
    } else {
        glBindBuffer(GL_ARRAY_BUFFER, mPosStartBuffer);
        glBufferData(GL_ARRAY_BUFFER, 4 * 3 * sizeof(GLfloat), posStartData, GL_STATIC_DRAW);
        glEnableVertexAttribArray(mAttribPosStart);
        glVertexAttribPointer(mAttribPosStart, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    }

    if (mAttribPosEnd == -1) {
        std::cerr << "warning: laser shader discarding pos end data\n";
    } else {
        glBindBuffer(GL_ARRAY_BUFFER, mPosEndBuffer);
        glBufferData(GL_ARRAY_BUFFER, 4 * 3 * sizeof(GLfloat), posEndData, GL_STATIC_DRAW);
        glEnableVertexAttribArray(mAttribPosEnd);
        glVertexAttribPointer(mAttribPosEnd, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    }

    if (mAttribOffset == -1) {
        std::cerr << "warning: laser shader discarding offset data\n";
    } else {
        glBindBuffer(GL_ARRAY_BUFFER, mOffsetBuffer);
        glBufferData(GL_ARRAY_BUFFER, 4 * 1 * sizeof(GLfloat), offsetData, GL_STATIC_DRAW);
        glEnableVertexAttribArray(mAttribOffset);
        glVertexAttribPointer(mAttribOffset, 1, GL_FLOAT, GL_FALSE, 0, NULL);
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

    glBindVertexArray(mVertexArray);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

}
