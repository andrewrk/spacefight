#include "shader.h"

#include <sstream>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cassert>

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

GLuint Shader::uniformId(const char *name)
{
    return glGetUniformLocation(mProgramId, name);
}

void Shader::bindAttribLocation(GLuint index, const std::string &name)
{
    glBindAttribLocation(mProgramId, index, name.c_str());
}

