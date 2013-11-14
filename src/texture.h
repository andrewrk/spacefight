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

        GLuint getID() { return mID; }
        void setFiltering(GLenum target, GLenum name, GLint param);
       // void setSamplerParameter(GLenum target, )

    private:
        GLuint mID;
        std::string mPath;
        bool load();
        SDL_Surface* inverse(SDL_Surface *source);



};

#endif // TEXTURE_H
