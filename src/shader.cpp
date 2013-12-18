#include "shader.h"

#include "resourcebundle.h"

#include <sstream>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cassert>
#include <cstring>

static void initShader(const ResourceBundle *bundle, const std::string &sourcePath, GLuint &shaderId, GLenum type) {
    shaderId = glCreateShader(type);
    std::string sourceText = bundle->getString(sourcePath);
    assert(sourceText.length() > 0);
    const char * sourceTextCStr = sourceText.c_str();
    glShaderSource(shaderId, 1, &sourceTextCStr, NULL);
    glCompileShader(shaderId);

    GLint ok;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &ok);
    if (ok)
        return;

    GLint errorSize;
    glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &errorSize);

    char* errorString = new char[errorSize + 1];
    glGetShaderInfoLog(shaderId, errorSize, &errorSize, errorString);
    errorString[errorSize] = '\0';

    std::cerr << "Error compiling shader:" << sourcePath << ":\n" << errorString << std::endl;

    exit(1);
}


Shader::Shader(const std::string &vsFilename, const std::string &fsFilename, const ResourceBundle *bundle)
{
    initShader(bundle, vsFilename, mVertexId, GL_VERTEX_SHADER);
    initShader(bundle, fsFilename, mFragmentId, GL_FRAGMENT_SHADER);

    mProgramId = glCreateProgram();
    glAttachShader(mProgramId, mFragmentId);
    glAttachShader(mProgramId, mVertexId);
    glLinkProgram(mProgramId);

    GLint ok;
    glGetProgramiv(mProgramId, GL_LINK_STATUS, &ok);

    if (ok)
        return;

    GLint errorSize;
    glGetProgramiv(mProgramId, GL_INFO_LOG_LENGTH, &errorSize);

    char* errorString = new char[errorSize + 1];
    glGetProgramInfoLog(mProgramId, errorSize, &errorSize, errorString);
    errorString[errorSize] = '\0';

    std::cerr << "Error linking shader program: " << errorString << "\n"
              << vsFilename << "\n" << fsFilename << "\n";
    exit(1);
}

Shader::~Shader()
{
    glDetachShader(mProgramId, mFragmentId);
    glDetachShader(mProgramId, mVertexId);

    glDeleteShader(mFragmentId);
    glDeleteShader(mVertexId);
    glDeleteProgram(mProgramId);
}

void Shader::bind()
{
    glUseProgram(mProgramId);
}

GLuint Shader::uniformId(const std::string &name)
{
    return glGetUniformLocation(mProgramId, name.c_str());
}

void Shader::setUniform(GLuint uniformId, int value)
{
    glUniform1i(uniformId, value);
}

void Shader::setUniform(GLuint uniformId, float value)
{
    glUniform1f(uniformId, value);
}

void Shader::setUniform(GLuint uniformId, const glm::vec3 &value)
{
    glUniform3fv(uniformId, 1, &value[0]);
}

void Shader::setUniform(GLuint uniformId, const glm::vec4 &value)
{
    glUniform4fv(uniformId, 1, &value[0]);
}

void Shader::setUniform(GLuint uniformId, const glm::mat4 &value)
{
    glUniformMatrix4fv(uniformId, 1, GL_FALSE, &value[0][0]);
}

void Shader::setUniform(GLuint uniformId, const glm::mat3 &value)
{
    glUniformMatrix3fv(uniformId, 1, GL_FALSE, &value[0][0]);
}

GLint Shader::attribLocation(const std::string &name)
{
    return glGetAttribLocation(mProgramId, name.c_str());
}

std::unique_ptr<Shader::UniformBlock> Shader::getUniformBlock(const std::string &blockName, const GLchar** fieldNames, GLuint bindingId)
{
    GLsizei count = 0;
    while (fieldNames[count])
        count += 1;

    std::unique_ptr<Shader::UniformBlock> uniformBlockPtr(new UniformBlock);

    GLenum err = glGetError();
    assert(err == GL_NO_ERROR);

    GLuint blockIndex = glGetUniformBlockIndex(mProgramId, blockName.c_str());
    assert(blockIndex != GL_INVALID_INDEX);
    glGetActiveUniformBlockiv(mProgramId, blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &uniformBlockPtr->mBlockSize);
    err = glGetError();
    assert(err == GL_NO_ERROR);
    uniformBlockPtr->mBuffer = new GLubyte[uniformBlockPtr->mBlockSize];

    std::vector<GLuint> indices;
    indices.resize(count);
    glGetUniformIndices(mProgramId, count, fieldNames, &indices[0]);
    err = glGetError();
    assert(err == GL_NO_ERROR);


    // this one has the invalid indexes removed; uniformBlockPtr->mOffsets contains them as -1
    std::vector<GLint> offsets(count);
    uniformBlockPtr->mOffsets.resize(count);
    // mark the ones not found in the shader
    // and remove them from indices
    for (int i = 0, indexIndex = 0; i < count; i += 1, indexIndex += 1) {
        if (indices[i] == GL_INVALID_INDEX) {
            uniformBlockPtr->mOffsets[i] = -1;
            indices.erase(indices.begin() + indexIndex);
            offsets.erase(offsets.begin() + indexIndex);
            indexIndex -= 1;
        } else {
            // set to zero so that later we can tell the difference between
            // missing offsets and normal offsets
            uniformBlockPtr->mOffsets[i] = 0;
        }
    }

    glGetActiveUniformsiv(mProgramId, offsets.size(), &indices[0], GL_UNIFORM_OFFSET, &offsets[0]);
    err = glGetError();
    assert(err == GL_NO_ERROR);

    glGenBuffers(1, &uniformBlockPtr->mUboHandle);
    glBindBuffer(GL_UNIFORM_BUFFER, uniformBlockPtr->mUboHandle);
    glBufferData(GL_UNIFORM_BUFFER, uniformBlockPtr->mBlockSize, uniformBlockPtr->mBuffer, GL_DYNAMIC_DRAW);

    err = glGetError();
    assert(err == GL_NO_ERROR);
    glBindBufferBase(GL_UNIFORM_BUFFER, blockIndex, uniformBlockPtr->mUboHandle);

    err = glGetError();
    assert(err == GL_NO_ERROR);

    glUniformBlockBinding(mProgramId, blockIndex, bindingId);
    err = glGetError();
    assert(err == GL_NO_ERROR);

    // update mOffsets with the correct values
    for (int i = 0, offsetIndex = 0; i < count; i += 1, offsetIndex += 1) {
        GLint offset = uniformBlockPtr->mOffsets[i];
        if (offset == -1) {
            offsetIndex -= 1;
            continue;
        }
        uniformBlockPtr->mOffsets[i] = offsets[offsetIndex];
    }

    return uniformBlockPtr;
}

void Shader::debugPrintAttributes()
{
    GLint maxLength, nAttribs;
    glGetProgramiv(mProgramId, GL_ACTIVE_ATTRIBUTES, &nAttribs);
    glGetProgramiv(mProgramId, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxLength);

    GLchar *name = new GLchar[maxLength];

    std::cerr << "Index, Name\n";
    std::cerr << "----------\n";
    GLint written, size, location;
    GLenum type;
    for (int i = 0; i < nAttribs; i += 1) {
        glGetActiveAttrib(mProgramId, i, maxLength, &written, &size, &type, name);
        location = glGetAttribLocation(mProgramId, name);
        std::cerr << location << ", " << name << "\n";
    }

    delete name;


    GLenum err = glGetError();
    assert(err == GL_NO_ERROR);

}

void Shader::UniformBlock::set(int index, float value)
{
    GLint offset = mOffsets[index];
    if (offset == -1) return;
    GLubyte *bytePtr = mBuffer + offset;
    float *floatPtr = reinterpret_cast<float *>(bytePtr);
    *floatPtr = value;
}

void Shader::UniformBlock::set(int index, const glm::vec3 &value)
{
    GLint offset = mOffsets[index];
    if (offset == -1) return;
    GLubyte *bytePtr = mBuffer + offset;
    glm::vec3 *vecPtr = reinterpret_cast<glm::vec3 *>(bytePtr);
    *vecPtr = value;
}

void Shader::UniformBlock::set(int index, const glm::vec4 &value)
{
    GLint offset = mOffsets[index];
    if (offset == -1) return;
    GLubyte *bytePtr = mBuffer + offset;
    glm::vec4 *vecPtr = reinterpret_cast<glm::vec4 *>(bytePtr);
    *vecPtr = value;
}

void Shader::UniformBlock::update()
{
    glBindBuffer(GL_UNIFORM_BUFFER, mUboHandle);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, mBlockSize, mBuffer);
}


Shader::UniformBlock::~UniformBlock()
{
    glDeleteBuffers(1, &mUboHandle);
    delete[] mBuffer;
}
