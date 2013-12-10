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
    int err = rucksack_file_read(entry, reinterpret_cast<unsigned char *>(&contents[0]));

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

    int err = rucksack_file_read(entry, &buffer[0]);

    if (err) {
        std::cerr << "Error reading '" << key << "' resource: " << rucksack_err_str(err) << "\n";
        exit(1);
    }
}

void ResourceBundle::getImage(const std::string &key, Image &image)
{
    image.cleanup();

    getFileBuffer(key, image.mCompressedBytes);

    if (!image.buildFromCompressedBytes()) {
        std::cerr << "Error reading image from '" << key << "'\n";
        exit(1);
    }

}

void ResourceBundle::getSpriteSheet(const std::string &key, SpriteSheet &spritesheet)
{
    spritesheet.cleanup();
    spritesheet.init(this);

    // get the texture entry
    RuckSackFileEntry *entry = rucksack_bundle_find_file(mBundle, key.c_str());
    if (!entry) {
        std::cerr << "Could not find resource '" << key << "' in bundle.\n";
        exit(1);
    }
    RuckSackTexture *rsTexture;
    int err = rucksack_file_open_texture(entry, &rsTexture);
    if (err) {
        std::cerr << "Error reading '" << key << "' as texture: " << rucksack_err_str(err) << "\n";
        exit(1);
    }

    // read the texture image
    Image texImage;
    long size = rucksack_texture_size(rsTexture);
    texImage.mCompressedBytes.resize(size);
    err = rucksack_texture_read(rsTexture, &texImage.mCompressedBytes[0]);
    if (err) {
        std::cerr << "Error reading texture '" << key << "' from resources: " << rucksack_err_str(err) << "\n";
        exit(1);
    }
    if (!texImage.buildFromCompressedBytes()) {
        std::cerr << "Error decoding texture '" << key << "' from resources\n";
        exit(1);
    }

    // make the opengl texture for it
    glGenTextures(1, &spritesheet.mGlTex);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, spritesheet.mGlTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    texImage.doPixelStoreAlignment();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, texImage.width(), texImage.height(), 0, texImage.format(), texImage.type(), texImage.pixels());
    texImage.resetPixelStoreAlignment();

    // read the images metadata
    std::vector<RuckSackImage*> images;
    images.resize(rucksack_texture_image_count(rsTexture));
    rucksack_texture_get_images(rsTexture, &images[0]);
    GLfloat fWidth = texImage.width();
    GLfloat fHeight = texImage.height();
    for (unsigned int i = 0; i < images.size(); i += 1) {
        RuckSackImage *image = images[i];
        SpriteSheet::ImageInfo *info = &spritesheet.mInfoDict[image->name];
        info->x = image->x;
        info->y = image->y;
        info->width = image->width;
        info->height = image->height;
        info->anchorX = image->anchor_x;
        info->anchorY = image->anchor_y;
        info->r90 = image->r90;

        glGenVertexArrays(1, &info->vertexArray);
        glBindVertexArray(info->vertexArray);

        glGenBuffers(1, &info->vertexBuffer);
        glGenBuffers(1, &info->texCoordBuffer);

        if (spritesheet.attribPosition == -1) {
            std::cerr << "warning: shader discarding vertex buffer data\n";
        } else {
            GLfloat vertexes[4][3];
            if (info->r90) {
                vertexes[0][0] = info->width;
                vertexes[0][1] = 0.0f;
                vertexes[0][2] = 0.0f;

                vertexes[1][0] = 0.0f;
                vertexes[1][1] = 0.0f;
                vertexes[1][2] = 0.0f;

                vertexes[2][0] = info->width;
                vertexes[2][1] = info->height;
                vertexes[2][2] = 0.0f;

                vertexes[3][0] = 0.0f;
                vertexes[3][1] = info->height;
                vertexes[3][2] = 0.0f;

            } else {
                vertexes[0][0] = 0.0f;
                vertexes[0][1] = 0.0f;
                vertexes[0][2] = 0.0f;

                vertexes[1][0] = 0.0f;
                vertexes[1][1] = info->height;
                vertexes[1][2] = 0.0f;

                vertexes[2][0] = info->width;
                vertexes[2][1] = 0.0f;
                vertexes[2][2] = 0.0f;

                vertexes[3][0] = info->width;
                vertexes[3][1] = info->height;
                vertexes[3][2] = 0.0f;
            }
            glBindBuffer(GL_ARRAY_BUFFER, info->vertexBuffer);
            glBufferData(GL_ARRAY_BUFFER, 4 * 3 * sizeof(GLfloat), vertexes, GL_STATIC_DRAW);
            glEnableVertexAttribArray(spritesheet.attribPosition);
            glVertexAttribPointer(spritesheet.attribPosition, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        }

        if (spritesheet.attribTexCoord == -1) {
            std::cerr << "warning: shader discarding tex coord data\n";
        } else {
            GLfloat coords[4][2];
            if (info->r90) {
                coords[0][0] = (info->x + info->width) / fWidth;
                coords[0][1] = info->y / fHeight;

                coords[1][0] = info->x / fWidth;
                coords[1][1] = info->y / fHeight;

                coords[2][0] = (info->x + info->width) / fWidth;
                coords[2][1] = (info->y + info->height) / fHeight;

                coords[3][0] = info->x / fWidth;
                coords[3][1] = (info->y + info->height) / fHeight;
            } else {
                coords[0][0] = info->x / fWidth;
                coords[0][1] = info->y / fHeight;

                coords[1][0] = info->x / fWidth;
                coords[1][1] = (info->y + info->height) / fHeight;

                coords[2][0] = (info->x + info->width) / fWidth;
                coords[2][1] = info->y / fHeight;

                coords[3][0] = (info->x + info->width) / fWidth;
                coords[3][1] = (info->y + info->height) / fHeight;

            }
            glBindBuffer(GL_ARRAY_BUFFER, info->texCoordBuffer);
            glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(GLfloat), coords, GL_STATIC_DRAW);
            glEnableVertexAttribArray(spritesheet.attribTexCoord);
            glVertexAttribPointer(spritesheet.attribTexCoord, 2, GL_FLOAT, GL_FALSE, 0, NULL);
        }
    }

    rucksack_texture_close(rsTexture);
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
    unsigned int redMask = FreeImage_GetRedMask(mBmp);
    unsigned int greenMask = FreeImage_GetGreenMask(mBmp);
    unsigned int blueMask = FreeImage_GetBlueMask(mBmp);
    char hasAlpha = FreeImage_IsTransparent(mBmp);

    if (redMask == 0x0000ff00 && greenMask == 0x00ff0000 && blueMask == 0xff000000 && hasAlpha) {
        return GL_RGBA;
    } else if (redMask == 0x0000ff && greenMask == 0x00ff00 && blueMask == 0xff0000 && !hasAlpha) {
        return GL_RGB;
    } else if (redMask == 0xff0000 && greenMask == 0x00ff00 && blueMask == 0x0000ff) {
        return hasAlpha ? GL_BGRA : GL_BGR;
    } else {
        std::cerr << "unrecognized image format\n";
        exit(1);
    }
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

bool ResourceBundle::Image::buildFromCompressedBytes()
{

    mMem = FreeImage_OpenMemory(&mCompressedBytes[0], mCompressedBytes.size());
    FREE_IMAGE_FORMAT fif = FreeImage_GetFileTypeFromMemory(mMem, 0);

    if (fif == FIF_UNKNOWN || !FreeImage_FIFSupportsReading(fif)) {
        return false;
    }

    mBmp = FreeImage_LoadFromMemory(fif, mMem, 0);
    return FreeImage_HasPixels(mBmp);
}
