#include "boundarysphere.h"

#include "octahedron.h"
#include "resourcebundle.h"

#include <glm/glm.hpp>
#include <iostream>

BoundarySphere::BoundarySphere(ResourceBundle *bundle)
{
    mShader = bundle->getShader("boundarysphere");



    mShaderModelViewMatrix = mShader->uniformId("ModelViewMatrix");
    mShaderNormalMatrix = mShader->uniformId("NormalMatrix");
    mShaderProjectionMatrix = mShader->uniformId("ProjectionMatrix");
    mShaderMvp = mShader->uniformId("MVP");
    texUniformId = mShader->uniformId("Tex");

    attribPositionIndex = mShader->attribLocation("VertexPosition");
    attribNormalIndex = mShader->attribLocation("VertexNormal");

}

BoundarySphere::~BoundarySphere()
{
    cleanup();
}

void BoundarySphere::generate(int rowCount)
{
    cleanup();

    std::vector<glm::vec3> points;
    std::vector<GLuint> reverseIndexes;

    Octahedron::generate(rowCount, points, reverseIndexes);


    // generate spherical normals
    std::vector<glm::vec3> normals;
    for (unsigned int i = 0; i < points.size(); i += 1) {
        glm::vec3 normal = glm::normalize(points[i]);
        normals.push_back(normal);
        points[i] = normal;
    }

    // reverse the the winding order so that we can see the inside
    std::vector<GLuint> indexes;
    indexes.reserve(reverseIndexes.size());
    for (int i = reverseIndexes.size() - 1; i >= 0; i -= 1) {
        indexes.push_back(reverseIndexes[i]);
    }

    elementCount = indexes.size();

    glGenVertexArrays(1, &vertexArrayObject);
    glGenBuffers(1, &vertexPositionBuffer);
    glGenBuffers(1, &vertexNormalBuffer);
    glGenBuffers(1, &vertexIndexBuffer);


    glBindVertexArray(vertexArrayObject);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexIndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexes.size() * sizeof(GLuint), &indexes[0], GL_STATIC_DRAW);


    if (attribPositionIndex != -1) {
        glBindBuffer(GL_ARRAY_BUFFER, vertexPositionBuffer);
        glBufferData(GL_ARRAY_BUFFER, points.size() * 3 * sizeof(GLfloat), &points[0], GL_STATIC_DRAW);

        glEnableVertexAttribArray(attribPositionIndex);
        glVertexAttribPointer(attribPositionIndex, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    } else {
        std::cerr << "warning: shader ignoring vertex position data\n";
    }

    if (attribNormalIndex != -1) {
        glBindBuffer(GL_ARRAY_BUFFER, vertexNormalBuffer);
        glBufferData(GL_ARRAY_BUFFER, normals.size() * 3 * sizeof(GLfloat), &normals[0], GL_STATIC_DRAW);

        glEnableVertexAttribArray(attribNormalIndex);
        glVertexAttribPointer(attribNormalIndex, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    } else {
        std::cerr << "warning: shader ignoring vertex normal data\n";
    }

    GLenum err = glGetError();
    assert(err == GL_NO_ERROR);

}

void BoundarySphere::draw(const RenderContext &renderContext)
{
    mShader->bind();
    mShader->setUniform(mShaderMvp, renderContext.mvp);
    mShader->setUniform(mShaderModelViewMatrix, renderContext.modelView);
    mShader->setUniform(mShaderProjectionMatrix, renderContext.projection);
    mShader->setUniform(mShaderNormalMatrix, renderContext.normal);
    mShader->setUniform(texUniformId, 0);

    glBindVertexArray(vertexArrayObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexIndexBuffer);
    glDrawElements(GL_TRIANGLE_STRIP, elementCount, GL_UNSIGNED_INT, NULL);

}

void BoundarySphere::cleanup()
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
    if (vertexArrayObject) {
        glDeleteVertexArrays(1, &vertexArrayObject);
        vertexArrayObject = 0;
    }

}
