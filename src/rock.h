#ifndef ROCK_H
#define ROCK_H

#include "GL/glew.h"

#include "shader.h"
#include "rendercontext.h"
#include "drawable.h"

#include <memory>

class ResourceBundle;
class RockGenerator;

class Rock : public Drawable
{
public:
    Rock();
    ~Rock();

    void generate();

    void draw(const RenderContext &renderContext) const;

private:
    const RockGenerator *generator;

    GLuint vertexArrayObject = 0;
    GLuint vertexPositionBuffer = 0;
    GLuint vertexNormalBuffer = 0;
    GLuint vertexIndexBuffer = 0;
    GLuint texCoordBuffer = 0;



    int elementCount;

    void cleanup();

    friend class RockGenerator;
};

#endif // ROCK_H
