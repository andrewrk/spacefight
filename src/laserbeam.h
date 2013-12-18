#ifndef LASERBEAM_H
#define LASERBEAM_H

#include "rendercontext.h"
#include "shader.h"
#include "drawable.h"
class ResourceBundle;

class LaserBeam : public Drawable
{
public:
    LaserBeam(ResourceBundle *bundle);
    ~LaserBeam();

    void draw(const RenderContext &renderContext) const;

private:

    Shader *mShader;

    GLint mAttribPosStart;
    GLint mAttribPosEnd;
    GLint mAttribOffset;

    GLint mUniformBeamColor;
    GLint mUniformMvp;

    int mPointCount;

    GLuint mVertexArray;
    GLuint mPosStartBuffer;
    GLuint mPosEndBuffer;
    GLuint mOffsetBuffer;



};

#endif // LASERBEAM_H
