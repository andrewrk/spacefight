#ifndef TEXTURE_H
#define TEXTURE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <GL/glew.h>

#include <iostream>
#include <string>


class Texture
{
    public:
        Texture(std::string file);
        ~Texture();

        GLuint getID() { return mID; }
        bool load();
        void setParameter(GLenum parameter, GLenum value);
        void bind();
        void unbind();

    private:
        GLuint mID;
        std::string mPath;
        SDL_Surface* inverse(SDL_Surface *source) const;



};

#endif // TEXTURE_H
