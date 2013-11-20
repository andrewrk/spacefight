#ifndef SCENE_H
#define SCENE_H


#include <glm/glm.hpp>

#include "shader.h"
#include "model.h"
#include "rendercontext.h"

#include <SDL2/SDL.h>
#include <GL/glew.h>

#include "texture.h"
#include "skybox.h"

#include "label.h"

class Scene
{
public:
    Scene();
    ~Scene();
    int start();
    void setMinFps(double fps);
private:
    bool mRunning = true;
    SDL_GLContext mContext;
    SDL_Window *mWindow;

    RenderContext mRenderContext;


    Model *mMonkeyModel;

    Skybox *mSkybox;


    double mMaxSpf;
    double mFps;
    Label *mFpsLabel;

    std::vector<SDL_Joystick*> mJoysticks;

    float mCameraDistance = 8.0;
    glm::vec3 mCameraDirection; // unit vector pointing from 0,0,0 to camera

    void flushEvents();
    void update(double dt, double dx);
    void draw();
    void initJoystick();



};

#endif // SCENE_H
