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

void ResourceBundle::getImage(const std::string &key, Image &image)
{
    image.cleanup();

    getFileBuffer(key, image.mCompressedBytes);

    image.mMem = FreeImage_OpenMemory(&image.mCompressedBytes[0], image.mCompressedBytes.size());
    FREE_IMAGE_FORMAT fif = FreeImage_GetFileTypeFromMemory(image.mMem, 0);

    if (fif == FIF_UNKNOWN || !FreeImage_FIFSupportsReading(fif)) {
        std::cerr << "Unknown image format for key '" << key << "'\n";
        exit(1);
    }

    image.mBmp = FreeImage_LoadFromMemory(fif, image.mMem, 0);
}

ResourceBundle::Image::~Image()
{
    cleanup();
}

int ResourceBundle::Image::width() const
{
    return FreeImage_GetWidth(mBmp);
}

int ResourceBundle::Image::height() const
{
    return FreeImage_GetHeight(mBmp);
}

void ResourceBundle::Image::doPixelStoreAlignment()
{
    glPixelStorei(GL_PACK_ALIGNMENT, 4);
}

void ResourceBundle::Image::resetPixelStoreAlignment()
{
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
}

GLenum ResourceBundle::Image::format() const
{
    return GL_BGRA;
}

GLenum ResourceBundle::Image::type() const
{
    return GL_UNSIGNED_BYTE;
}

unsigned char *ResourceBundle::Image::pixels() const
{
    return FreeImage_GetBits(mBmp);
}

void ResourceBundle::Image::cleanup()
{
    if (mBmp) {
        FreeImage_Unload(mBmp);
        mBmp = NULL;
    }
    if (mMem) {
        FreeImage_CloseMemory(mMem);
        mMem = NULL;
    }
}
