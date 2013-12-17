#ifndef BOUNDARYSPHERE_H
#define BOUNDARYSPHERE_H

#include "shader.h"
#include "rendercontext.h"

class ResourceBundle;

class BoundarySphere
{
public:
    BoundarySphere(ResourceBundle *bundle);
    ~BoundarySphere();


    void generate(int rowCount);
    void draw(const RenderContext &renderContext);

private:

    GLuint vertexArrayObject = 0;
    GLuint vertexPositionBuffer = 0;
    GLuint vertexNormalBuffer = 0;
    GLuint vertexIndexBuffer = 0;


    GLint attribPositionIndex;
    GLint attribNormalIndex;

    GLuint mShaderModelViewMatrix;
    GLuint mShaderNormalMatrix;
    GLuint mShaderProjectionMatrix;
    GLuint mShaderMvp;
    GLuint texUniformId;

    Shader *mShader;

    int elementCount;


    void cleanup();
};

#endif // BOUNDARYSPHERE_H
