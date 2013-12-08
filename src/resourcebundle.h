#ifndef RESOURCEBUNDLE_H
#define RESOURCEBUNDLE_H

#include "shader.h"
#include "spritesheet.h"

#include <rucksack.h>
#include <string>
#include <map>
#include <vector>


#include <FreeImage.h>

class ResourceBundle
{
public:
    ResourceBundle(const std::string &filename);
    ~ResourceBundle();

    std::string getString(const std::string &key) const;

    Shader *getShader(const std::string &name);

    void getFileBuffer(const std::string &key, std::vector<unsigned char> &buffer);

    class Image {
    public:
        Image() {}
        ~Image();

        int width() const;
        int height() const;
        void doPixelStoreAlignment();
        void resetPixelStoreAlignment();
        GLenum format() const;
        GLenum type() const;
        unsigned char *pixels() const;
    private:
        FIBITMAP *mBmp = NULL;
        FIMEMORY *mMem = NULL;
        std::vector<unsigned char> mCompressedBytes;

        void cleanup();
        bool buildFromCompressedBytes();
        friend class ResourceBundle;
    };

    void getImage(const std::string &key, Image &image);

    void getSpriteSheet(const std::string &key, SpriteSheet &spritesheet);



private:
    RuckSackBundle *mBundle;

    std::map<std::string, Shader*> mShaderDict = {};

};

#endif // RESOURCEBUNDLE_H
