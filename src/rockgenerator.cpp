#include "rockgenerator.h"
#include "lightblock.h"
#include "octahedron.h"

#include <iostream>

RockGenerator::RockGenerator(ResourceBundle *bundle)
{
    mShader = bundle->getShader("rock");

    mLightBlock = mShader->getUniformBlock("Light", LIGHT_BLOCK_FIELDS, 0);
    mMaterialBlock = mShader->getUniformBlock("Material", MATERIAL_BLOCK_FIELDS, 1);


    mShaderModelViewMatrix = mShader->uniformId("ModelViewMatrix");
    mShaderNormalMatrix = mShader->uniformId("NormalMatrix");
    mShaderProjectionMatrix = mShader->uniformId("ProjectionMatrix");
    mShaderMvp = mShader->uniformId("MVP");
    texUniformId = mShader->uniformId("Tex");

    attribPositionIndex = mShader->attribLocation("VertexPosition");
    attribNormalIndex = mShader->attribLocation("VertexNormal");
    attribTexCoordIndex = mShader->attribLocation("TexCoord");

    ResourceBundle::Image image;
    bundle->getImage("rockTexture", image);

    glGenTextures(1, &mTexture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    image.doPixelStoreAlignment();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width(), image.height(), 0, image.format(), image.type(), image.pixels());
    image.resetPixelStoreAlignment();


    GLenum err = glGetError();
    assert(err == GL_NO_ERROR);
}

RockGenerator::~RockGenerator()
{
    glDeleteTextures(1, &mTexture);
    mTexture = 0;
}




static const float PI = 3.14159265358979;

static float randFloat() {
    return rand() / (float) RAND_MAX;
}


void RockGenerator::generate(Rock &rock)
{
    rock.cleanup();

    rock.generator = this;

    std::vector<glm::vec3> points;
    std::vector<GLuint> indexes;

    Octahedron::generate(10, points, indexes);

    std::vector<glm::vec2> texCoords;
    std::vector<glm::vec3> normals;
    for (unsigned int i = 0; i < points.size(); i += 1) {
        glm::vec3 normal = glm::normalize(points[i]);
        normals.push_back(normal);

        points[i] = (0.9f + randFloat() * 0.2f) * normal;

        texCoords.push_back(glm::vec2(
                0.5 + atan2(normal[2], normal[0]) / (2 * PI),
                0.5 - asin(normal[1]) / PI));
    }

    rock.elementCount = indexes.size();

    glGenVertexArrays(1, &rock.vertexArrayObject);
    glGenBuffers(1, &rock.vertexPositionBuffer);
    glGenBuffers(1, &rock.vertexNormalBuffer);
    glGenBuffers(1, &rock.vertexIndexBuffer);
    glGenBuffers(1, &rock.texCoordBuffer);


    glBindVertexArray(rock.vertexArrayObject);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rock.vertexIndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexes.size() * sizeof(GLuint), &indexes[0], GL_STATIC_DRAW);


    if (attribPositionIndex != -1) {
        glBindBuffer(GL_ARRAY_BUFFER, rock.vertexPositionBuffer);
        glBufferData(GL_ARRAY_BUFFER, points.size() * 3 * sizeof(GLfloat), &points[0], GL_STATIC_DRAW);

        glEnableVertexAttribArray(attribPositionIndex);
        glVertexAttribPointer(attribPositionIndex, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    } else {
        std::cerr << "warning: shader ignoring vertex position data\n";
    }

    if (attribNormalIndex != -1) {
        glBindBuffer(GL_ARRAY_BUFFER, rock.vertexNormalBuffer);
        glBufferData(GL_ARRAY_BUFFER, normals.size() * 3 * sizeof(GLfloat), &normals[0], GL_STATIC_DRAW);

        glEnableVertexAttribArray(attribNormalIndex);
        glVertexAttribPointer(attribNormalIndex, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    } else {
        std::cerr << "warning: shader ignoring vertex normal data\n";
    }

    if (attribTexCoordIndex != -1) {
        glBindBuffer(GL_ARRAY_BUFFER, rock.texCoordBuffer);
        glBufferData(GL_ARRAY_BUFFER, texCoords.size() * 2 * sizeof(GLfloat), &texCoords[0], GL_STATIC_DRAW);

        glEnableVertexAttribArray(attribTexCoordIndex);
        glVertexAttribPointer(attribTexCoordIndex, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    } else {
        std::cerr << "Warning: shader ignoring tex coord data\n";
    }

    GLenum err = glGetError();
    assert(err == GL_NO_ERROR);
}
