#ifndef SCENE_H
#define SCENE_H


#include <glm/glm.hpp>

#include "shader.h"
#include "model.h"
#include "rendercontext.h"

#include <SDL2/SDL.h>
#include <GL/glew.h>

class Scene
{
public:
    Scene();
    ~Scene();
    int start();
private:
    bool mRunning = true;
    SDL_GLContext mContext;
    SDL_Window *mWindow;

    RenderContext mRenderContext;


    Model *mMonkeyModel;


    std::vector<SDL_Joystick*> mJoysticks;

    void flushEvents();
    void draw();
    void initJoystick();

};

#endif // SCENE_H
