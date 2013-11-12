#include "shadermanager.h"
#include "shader.h"

#include <map>
#include <sstream>

static std::map<std::string, Shader*> dict = {};

Shader *ShaderManager::getShader(const std::string &name)
{
    Shader *shader = dict[name];
    if (!shader) {
        std::stringstream vertex_path;
        vertex_path << "src/shaders/" << name << ".vert";
        std::stringstream fragment_path;
        fragment_path << "src/shaders/" << name << ".frag";
        shader = new Shader(vertex_path.str(), fragment_path.str());
        dict[name] = shader;
    }
    return shader;
}
