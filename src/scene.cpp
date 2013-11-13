#include "scene.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <iostream>

#include <limits>
#include <cstdint>

#include <SDL2/SDL_joystick.h>

const double CAMERA_SPEED = 0.02;
const double CAMERA_ROTATION_SPEED = 0.02;

Scene::Scene() : mTexture("assets/test.png")
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

    GLenum err = glGetError();
    // we expect to maybe get invalid enum due to glewExperimental
    assert(err == GL_INVALID_ENUM || err == GL_NO_ERROR);

    // set buffer swap with monitor's vertical refresh rate
    SDL_GL_SetSwapInterval(1);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);


    mCameraDirection = glm::vec3(0, 1.0, 0);

    mRenderContext.projection = glm::perspective(60.0, width / (double)height, 0.1, 100.0);
    mRenderContext.model = glm::mat4(1.0);
    mRenderContext.lightPosition = glm::vec4(1.0, -3.0, 0.0, 1.0);
    mRenderContext.lightIntensityAmbient = glm::vec3(0.2, 0.2, 0.2);
    mRenderContext.lightIntensityDiffuse = glm::vec3(0.5, 0.5, 0.5);
    mRenderContext.lightIntensitySpecular = glm::vec3(0.8, 0.8, 0.8);
    mRenderContext.materialReflectivityAmbient = glm::vec3(0.5, 0.5, 0.5);
    mRenderContext.materialReflectivityDiffuse = glm::vec3(0.5, 0.5, 0.5);
    mRenderContext.materialReflectivitySpecular = glm::vec3(0.5, 0.5, 0.5);
    mRenderContext.materialSpecularShininess = 100.0f;

    mMonkeyModel = new Model("models/monkey.obj", mRenderContext);


}

Scene::~Scene()
{

    delete mMonkeyModel;


    SDL_GL_DeleteContext(mContext);
    SDL_DestroyWindow(mWindow);
}

int Scene::start() {

    initJoystick();

    mTexture.load();

    while(mRunning)
    {
        update(1.0);
        draw();
        SDL_GL_SwapWindow(mWindow);
    }
    return 0;
}

void Scene::update(double dx)
{
    flushEvents();

    double joyX = 0.0;
    double joyY = 0.0;
    double joyZ = 0.0;
    for (unsigned int i = 0; i < mJoysticks.size(); i += 1) {
        SDL_Joystick *joystick = mJoysticks[i];

        Sint16 val = SDL_JoystickGetAxis(joystick, 0);
        joyX += (double) val / (double) INT16_MAX;

        val = SDL_JoystickGetAxis(joystick, 1);
        joyY += (double) val / (double) INT16_MAX;

        val = SDL_JoystickGetAxis(joystick, 2);
        joyZ += (double) val / (double) INT16_MAX;
    }
    const Uint8 *state = SDL_GetKeyboardState(NULL);
    if (state[SDL_SCANCODE_LEFT]) joyX -= 1.0;
    if (state[SDL_SCANCODE_RIGHT]) joyX += 1.0;
    if (state[SDL_SCANCODE_UP]) joyZ -= 1.0;
    if (state[SDL_SCANCODE_DOWN]) joyZ += 1.0;

    mCameraDistance += joyZ * CAMERA_SPEED * dx;

    glm::vec3 eye = mCameraDirection * mCameraDistance;

    double angle = atan2(mCameraDirection[1], mCameraDirection[0]);
    angle += joyX * CAMERA_ROTATION_SPEED * dx;

    eye[0] = mCameraDistance * cos(angle);
    eye[1] = mCameraDistance * sin(angle);

    mCameraDirection = glm::normalize(eye);


    mRenderContext.view = glm::lookAt(
                eye,
                glm::vec3(0.0, 0.0, 0.0),
                glm::vec3(0.0, 0.0, 1.0));

    mRenderContext.modelView = mRenderContext.view * mRenderContext.model;
    mRenderContext.mvp = mRenderContext.projection * mRenderContext.modelView;
    mRenderContext.normal = glm::inverseTranspose(glm::mat3(mRenderContext.modelView));

}

void Scene::draw()
{
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    mMonkeyModel->draw();
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

void Scene::initJoystick()
{
    int joystickCount = SDL_NumJoysticks();
    for(int i = 0; i < joystickCount; i++)
    {
        std::cout << "Joystick name: " << SDL_JoystickNameForIndex(i) << "\n";
        SDL_Joystick *joystick = SDL_JoystickOpen(i);

        if (!joystick) {
            std::cerr << "Unable to open joystick: " << SDL_GetError() << "\n";
        } else {
            mJoysticks.push_back(joystick);
        }
    }

    SDL_JoystickEventState(SDL_ENABLE);
}
