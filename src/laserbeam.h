#ifndef LASERBEAM_H
#define LASERBEAM_H

#include "rendercontext.h"
#include "shader.h"
class ResourceBundle;

class LaserBeam
{
public:
    LaserBeam(ResourceBundle *bundle);
    ~LaserBeam();

    void draw(const RenderContext &renderContext) const;

private:

    Shader *mShader;

    GLint mAttribVertexPosition;
    GLint mUniformBeamColor;
    GLint mUniformMvp;

    int mPointCount;

    GLuint mVertexArray;
    GLuint mVertexBuffer;



};

#endif // LASERBEAM_H
