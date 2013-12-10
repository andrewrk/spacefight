#include "rock.h"

#include "resourcebundle.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <vector>
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


Rock::Rock(ResourceBundle *bundle)
{
    mShader = bundle->getShader("basic");

    mLightBlock = mShader->getUniformBlock("Light", LIGHT_BLOCK_FIELDS, 0);
    mMaterialBlock = mShader->getUniformBlock("Material", MATERIAL_BLOCK_FIELDS, 1);


    mShaderModelViewMatrix = mShader->uniformId("ModelViewMatrix");
    mShaderNormalMatrix = mShader->uniformId("NormalMatrix");
    mShaderProjectionMatrix = mShader->uniformId("ProjectionMatrix");
    mShaderMvp = mShader->uniformId("MVP");

    attribPositionIndex = mShader->attribLocation("VertexPosition");
    attribNormalIndex = mShader->attribLocation("VertexNormal");
}

Rock::~Rock()
{
    cleanup();
}

static const float invSqrt2 = 0.7071067811865475; // 1/sqrt(2)

static void addOctahedronFace(std::vector<glm::vec3> &points,
                              std::vector<GLuint> &indexes,
                              const glm::mat3 &matrix)
{
    int rowCount = 10;

    float sideLen = 2 * invSqrt2 / (float)rowCount;

    int indexOffset = points.size();

    glm::vec3 top(0, 1, 0);
    glm::vec3 bottomLeft(invSqrt2, 0, -invSqrt2);
    glm::vec3 downLeftDir = glm::normalize(bottomLeft - top);
    glm::vec3 rightDir(0, 0, 1);
    for (int row = 0; row <= rowCount; row += 1) {
        glm::vec3 start = top + downLeftDir * (row * sideLen);
        for (int i = 0; i <= row; i += 1) {
            points.push_back(matrix * (start + rightDir * (i * sideLen)));
        }
    }

    for (int row = 0; row < rowCount; row += 1) {
        int triangleCount = row + 1;
        int topIndex = indexOffset + row * (row + 1) / 2;
        int bottomIndex = indexOffset + (row + 1) * (row + 2) / 2;
        // degenerate triangle to reset the strip
        if (indexes.size() > 0) {
            indexes.push_back(indexes[indexes.size() - 1]);
        }
        indexes.push_back(bottomIndex++);
        for (int i = 0; i < triangleCount; i += 1) {
            indexes.push_back(topIndex++);
            indexes.push_back(bottomIndex++);
        }
    }
}

static const float PI = 3.1415926535;

void Rock::generate()
{
    cleanup();

    std::vector<glm::vec3> points;
    std::vector<GLuint> indexes;


    addOctahedronFace(points, indexes, glm::mat3(1));
    addOctahedronFace(points, indexes, glm::mat3(glm::rotate(glm::mat4(1), PI / 2, glm::vec3(0, 1, 0))));
    addOctahedronFace(points, indexes, glm::mat3(glm::rotate(glm::mat4(1), PI, glm::vec3(0, 1, 0))));
    addOctahedronFace(points, indexes, glm::mat3(glm::rotate(glm::mat4(1), 3 * PI / 2, glm::vec3(0, 1, 0))));
    glm::mat4 matrix = glm::rotate(glm::mat4(1), PI, glm::vec3(0, 0, 1));
    addOctahedronFace(points, indexes, glm::mat3(matrix));
    addOctahedronFace(points, indexes, glm::mat3(glm::rotate(matrix, PI / 2, glm::vec3(0, 1, 0))));
    addOctahedronFace(points, indexes, glm::mat3(glm::rotate(matrix, PI, glm::vec3(0, 1, 0))));
    addOctahedronFace(points, indexes, glm::mat3(glm::rotate(matrix, 3 * PI / 2, glm::vec3(0, 1, 0))));

    std::vector<glm::vec3> normals;
    for (unsigned int i = 0; i < points.size(); i += 1) {
        normals.push_back(glm::normalize(points[i]));
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

void Rock::draw(const RenderContext &renderContext)
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

    glBindVertexArray(vertexArrayObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexIndexBuffer);
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
    if (vertexArrayObject) {
        glDeleteVertexArrays(1, &vertexArrayObject);
        vertexArrayObject = 0;
    }
}
