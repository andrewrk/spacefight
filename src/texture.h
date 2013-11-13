#ifndef TEXTURE_H
#define TEXTURE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <GL/glew.h>

//#include "SDL_image.h"
#include <iostream>
#include <string>


class Texture
{
    public:
        Texture(std::string file);
        ~Texture();

        bool load();
        GLuint getID() { return mID; }
        SDL_Surface* inverse(SDL_Surface *source);

    private:
        GLuint mID;
        std::string mPath;


};

#endif // TEXTURE_H
