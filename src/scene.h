#ifndef SCENE_H
#define SCENE_H


#include <glm/glm.hpp>

#include "shader.h"
#include "model.h"

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
    SDL_GLContext mContext;
    SDL_Window *mWindow;
    glm::mat4 mProjection;
    glm::mat4 mView;
    glm::mat4 mModel;
    glm::mat4 mMvp;


    Model *mMonkeyModel;

    void flushEvents();
    void draw();

    Input mInput;

};

#endif // SCENE_H
