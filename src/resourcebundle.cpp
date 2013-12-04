#include "resourcebundle.h"
#include <iostream>

#include <sstream>

ResourceBundle::ResourceBundle(const std::string &filename)
{
    int err = rucksack_bundle_open(filename.c_str(), &mBundle);
    if (err) {
        std::cerr << "Error opening resource bundle: " << rucksack_err_str(err) << "\n";
        exit(1);
    }
}

ResourceBundle::~ResourceBundle()
{
    rucksack_bundle_close(mBundle);
}

std::string ResourceBundle::getString(const std::string &key) const
{
    RuckSackFileEntry *entry = rucksack_bundle_find_file(mBundle, key.c_str());

    if (!entry) {
        std::cerr << "Could not find resource '" << key << "' in bundle.\n";
        exit(1);
    }

    long size = rucksack_file_size(entry);
    std::string contents;
    contents.resize(size);
    int err = rucksack_bundle_file_read(mBundle, entry, reinterpret_cast<unsigned char *>(&contents[0]));

    if (err) {
        std::cerr << "Error reading '" << key << "' resource: " << rucksack_err_str(err) << "\n";
        exit(1);
    }

    return contents;
}

Shader *ResourceBundle::getShader(const std::string &name)
{
    Shader *shader = mShaderDict[name];
    if (!shader) {
        std::stringstream vertex_path;
        vertex_path << "shader/" << name << ".vert";
        std::stringstream fragment_path;
        fragment_path << "shader/" << name << ".frag";
        shader = new Shader(vertex_path.str(), fragment_path.str(), this);
        mShaderDict[name] = shader;
    }
    return shader;
}

void ResourceBundle::getFileBuffer(const std::string &key, std::vector<unsigned char> &buffer)
{
    RuckSackFileEntry *entry = rucksack_bundle_find_file(mBundle, key.c_str());

    if (!entry) {
        std::cerr << "Could not find resource '" << key << "' in bundle.\n";
        exit(1);
    }

    long size = rucksack_file_size(entry);

    buffer.resize(size);

    int err = rucksack_bundle_file_read(mBundle, entry, &buffer[0]);

    if (err) {
        std::cerr << "Error reading '" << key << "' resource: " << rucksack_err_str(err) << "\n";
        exit(1);
    }
}
