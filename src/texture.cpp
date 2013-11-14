#include "texture.h"

Texture::Texture(std::string file)
    :mID(0), mPath(file)
{
    load();
}

Texture::~Texture()
{

}

bool Texture::load()
{
    SDL_Surface *image = IMG_Load(mPath.c_str());

    if(image == 0)
    {
        std::cout << "Error: " << SDL_GetError() << std::endl;
        return false;
    }

    SDL_Surface *inversed = inverse(image);
    SDL_FreeSurface(image);

    if(glIsTexture(mID) == GL_TRUE)
        glDeleteTextures(1, &mID);

    glGenTextures(1, &mID);
    glBindTexture(GL_TEXTURE_2D, mID);
    {
        GLenum internalFormat(0); //RGB or RGBA
        GLenum format(0);

        if(inversed->format->BytesPerPixel == 3) //No alpha
            internalFormat = GL_RGB;
        else if(inversed->format->BytesPerPixel == 4)
            internalFormat = GL_RGBA;
        else
        {
            std::cout << "Error, unknown image format" << std::endl;
            SDL_FreeSurface(inversed);
            return false;
        }

        //Copies the pixels
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, inversed->w, inversed->h, 0, format, GL_UNSIGNED_BYTE, inversed->pixels);
        //Filters for texture resolution
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //Closest
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); //Further away
    }
    glBindTexture(GL_TEXTURE_2D, 0);
}

SDL_Surface* Texture::inverse(SDL_Surface* source)
{
    SDL_Surface* inversed;

    inversed = SDL_CreateRGBSurface(0, source->w, source->h, source->format->BitsPerPixel,
                                    source->format->Rmask, source->format->Gmask,
                                    source->format->Bmask, source->format->Amask);

    unsigned char* pixelsSource = (unsigned char*)source->pixels;
    unsigned char* pixelsInversed = (unsigned char*)inversed->pixels;

    for(int i(0); i < source->h; i++)
    {
        for(int j(0); j < source->w * source->format->BytesPerPixel; j++)
        {
            pixelsInversed[(source->w * source->format->BytesPerPixel * (source->h - 1 - i)) + j] = pixelsSource[(source->w * source->format->BytesPerPixel * i) + j];
        }
    }

    return inversed;
}
