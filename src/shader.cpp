#include "shader.h"

#include <sstream>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cassert>
#include <cstring>

static std::string textFileRead(std::string filename) {
    std::ifstream t(filename);
    std::stringstream buffer;
    buffer << t.rdbuf();
    return buffer.str();
}


static void initShader(const std::string &sourcePath, GLuint &shaderId, GLenum type) {
    shaderId = glCreateShader(type);
    std::string sourceText = textFileRead(sourcePath);
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


Shader::Shader(const std::string &vsFilename, const std::string &fsFilename)
{
    initShader(vsFilename, mVertexId, GL_VERTEX_SHADER);
    initShader(fsFilename, mFragmentId, GL_FRAGMENT_SHADER);

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

void Shader::unbind()
{
    glUseProgram(0);
}

GLuint Shader::uniformId(const std::string &name)
{
    return glGetUniformLocation(mProgramId, name.c_str());
}

GLint Shader::attribLocation(const std::string &name)
{
    return glGetAttribLocation(mProgramId, name.c_str());
}

std::unique_ptr<Shader::UniformBlock> Shader::getUniformBlock(const std::string &blockName, const GLchar** fieldNames)
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

    GLuint *indices = new GLuint[count];
    glGetUniformIndices(mProgramId, count, fieldNames, indices);
    err = glGetError();
    assert(err == GL_NO_ERROR);
    uniformBlockPtr->mOffsets.resize(count);
    glGetActiveUniformsiv(mProgramId, count, indices, GL_UNIFORM_OFFSET, &uniformBlockPtr->mOffsets[0]);
    err = glGetError();
    assert(err == GL_NO_ERROR);

    glGenBuffers(1, &uniformBlockPtr->mUboHandle);
    err = glGetError();
    assert(err == GL_NO_ERROR);
    glBindBufferBase(GL_UNIFORM_BUFFER, blockIndex, uniformBlockPtr->mUboHandle);

    err = glGetError();
    assert(err == GL_NO_ERROR);

    delete[] indices;

    return uniformBlockPtr;
}

void Shader::UniformBlock::set(int index, float value) {
    GLubyte *bytePtr = mBuffer + mOffsets[index];
    float *floatPtr = reinterpret_cast<float *>(bytePtr);
    *floatPtr = value;
}

void Shader::UniformBlock::set(int index, const glm::vec3 &value)
{
    GLubyte *bytePtr = mBuffer + mOffsets[index];
    memcpy(bytePtr, &value[0], sizeof(float) * 3);
}

void Shader::UniformBlock::set(int index, const glm::vec4 &value)
{
    GLubyte *bytePtr = mBuffer + mOffsets[index];
    memcpy(bytePtr, &value[0], sizeof(float) * 4);
}

void Shader::UniformBlock::update()
{
    glBindBuffer(GL_UNIFORM_BUFFER, mUboHandle);
    glBufferData(GL_UNIFORM_BUFFER, mBlockSize, mBuffer, GL_DYNAMIC_DRAW);
}


Shader::UniformBlock::~UniformBlock()
{
    glDeleteBuffers(1, &mUboHandle);
    delete[] mBuffer;
}
