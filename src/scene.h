#ifndef SCENE_H
#define SCENE_H

#include "shader.h"
#include <SDL2/SDL.h>
#include <GL/glew.h>

#include "Input.h"

class Scene
{
public:
    Scene();
    ~Scene();
    int start();
private:
    bool mRunning = true;
    GLuint mVertexArrayId;
    GLuint mVertexBuffer;
    SDL_GLContext mContext;
    SDL_Window *mWindow;
    Shader *shader;

    void flushEvents();
    void draw();

    Input mInput;

};

#endif // SCENE_H
