#ifndef SHADER_H
#define SHADER_H

#include <vector>
#include <string>
#include <memory>
#include <GL/glew.h>

#include <glm/glm.hpp>

class Shader
{
public:
    Shader(const std::string &vsFilename, const std::string &fsFilename);
    ~Shader();

    void bind();
    void unbind();

    GLuint uniformId(const std::string &name);
    GLint attribLocation(const std::string &name);

    class UniformBlock {
    public:
        void set(int index, float value);
        void set(int index, const glm::vec3 &value);
        void set(int index, const glm::vec4 &value);

        void update();

        ~UniformBlock();
    private:
        GLubyte *mBuffer;
        std::vector<GLint> mOffsets;
        GLint mBlockSize;
        GLuint mUboHandle;

        friend class Shader;
    };

    std::unique_ptr<UniformBlock> getUniformBlock(const std::string &blockName, const GLchar **fieldNames, GLuint bindingId);

private:
    GLuint mProgramId;
    GLuint mVertexId;
    GLuint mFragmentId;
};

#endif // SHADER_H
