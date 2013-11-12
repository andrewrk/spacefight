#ifndef SHADERMANAGER_H
#define SHADERMANAGER_H

#include "shader.h"

namespace ShaderManager
{
    Shader *getShader(const std::string &name);
};

#endif // SHADERMANAGER_H
