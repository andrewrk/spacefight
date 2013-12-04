#ifndef RESOURCEBUNDLE_H
#define RESOURCEBUNDLE_H

#include "shader.h"
#include <rucksack.h>
#include <string>
#include <map>
#include <vector>

class ResourceBundle
{
public:
    ResourceBundle(const std::string &filename);
    ~ResourceBundle();

    std::string getString(const std::string &key) const;

    Shader *getShader(const std::string &name);

    void getFileBuffer(const std::string &key, std::vector<unsigned char> &buffer);


private:
    RuckSackBundle *mBundle;

    std::map<std::string, Shader*> mShaderDict = {};

};

#endif // RESOURCEBUNDLE_H
