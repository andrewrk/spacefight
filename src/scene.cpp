#include "scene.h"

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

Scene::Scene()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0) {
        std::cerr << "Unable to initialize SDL\n";
        exit(1);
    }
    atexit(SDL_Quit);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    int width = 1366;
    int height = 768;
    mWindow = SDL_CreateWindow("Space Fight 3D!",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height,
        SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_OPENGL);
    if (!mWindow) {
        std::cerr << "Unable to create window\n";
        exit(1);
    }

    mContext = SDL_GL_CreateContext(mWindow);

    glewExperimental = GL_TRUE;
    GLenum status = glewInit();
    if (status != GLEW_OK) {
        std::cerr << "GLEW error:" << glewGetErrorString(status) << "\n";
        exit(1);
    }

    // set buffer swap with monitor's vertical refresh rate
    SDL_GL_SetSwapInterval(1);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);



    for(int i(0); i < SDL_NumJoysticks(); i++)
    {
        std::cout << "Joystick name: " << SDL_JoystickNameForIndex(i);
    }


    mProjection = glm::perspective(60.0, width / (double)height, 0.1, 100.0);
    mView = glm::lookAt(
                glm::vec3(0.0, -4.0, 0.0),
                glm::vec3(0.0, 0.0, 0.0),
                glm::vec3(0.0, 0.0, 1.0));
    mModel = glm::mat4(1.0);
    mMvp = mProjection * mView * mModel;


    mMonkeyModel = new Model("models/monkey.obj");


}

Scene::~Scene()
{

    delete mMonkeyModel;


    SDL_GL_DeleteContext(mContext);
    SDL_DestroyWindow(mWindow);
}

int Scene::start() {

    mInput.initJoystick();

    while(mInput.checkRunning())
    {
        mInput.update();
        draw();
        SDL_GL_SwapWindow(mWindow);
    }
    return 0;
}


void Scene::draw()
{
    mMonkeyModel->draw(mMvp);
}

void Scene::flushEvents() {
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE)
                mRunning = false;
            break;
        case SDL_QUIT:
            mRunning = false;
            break;
        }
    }
}
