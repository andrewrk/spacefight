#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <GL/glew.h>

class Shader
{
public:
    Shader(const std::string &vsFilename, const std::string &fsFilename);
    ~Shader();

    void bind();
    void unbind();

private:
    GLuint mProgramId;
    GLuint mVertexId;
    GLuint mFragmentId;
};

#endif // SHADER_H
