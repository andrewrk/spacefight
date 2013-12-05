#ifndef SHADER_H
#define SHADER_H

#include <vector>
#include <string>
#include <memory>
#include <GL/glew.h>

#include <glm/glm.hpp>

class ResourceBundle;

class Shader
{
public:
    Shader(const std::string &vsFilename, const std::string &fsFilename, const ResourceBundle *bundle);
    ~Shader();

    void bind();

    GLuint uniformId(const std::string &name);
    void setUniform(GLuint uniformId, int value);
    void setUniform(GLuint uniformId, const glm::vec3 &value);
    void setUniform(GLuint uniformId, const glm::vec4 &value);
    void setUniform(GLuint uniformId, const glm::mat4 &value);
    void setUniform(GLuint uniformId, const glm::mat3 &value);

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

    void debugPrintAttributes();

private:
    GLuint mProgramId;
    GLuint mVertexId;
    GLuint mFragmentId;
};

#endif // SHADER_H
