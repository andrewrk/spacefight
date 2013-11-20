#include "scene.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <iostream>

#include <limits>
#include <cstdint>
#include <ctime>
#include <sstream>
#include <chrono>

#include <SDL2/SDL_joystick.h>



const double CAMERA_SPEED = 0.08;
const double CAMERA_ROTATION_SPEED = 0.02;

const double EPSILON = 0.0000000001;
const double MAX_DISPLAY_FPS = 90000;
const double TARGET_FPS = 60.0;
const double TARGET_SPF = 1.0 / TARGET_FPS;
const double FPS_SMOOTHNESS = 0.9;
const double FPS_ONE_FRAME_WEIGHT = 1.0 - FPS_SMOOTHNESS;


Scene::Scene() : mSkybox(0)
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

    mRenderContext.screenWidth = 1366;
    mRenderContext.screenHeight = 768;

    mWindow = SDL_CreateWindow("Space Fight 3D!",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        mRenderContext.screenWidth, mRenderContext.screenHeight, SDL_WINDOW_OPENGL);
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

    Label::init();

    // set buffer swap with monitor's vertical refresh rate
    SDL_GL_SetSwapInterval(1);

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glDepthFunc(GL_LESS);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    mCameraDirection = glm::vec3(0, 1.0, 0);

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

    mFpsLabel = new Label(mRenderContext);
    mFpsLabel->setColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    mFpsLabel->setFontFace("Sans 12");

    mSkybox = new Skybox("assets", "front.png", "back.png", "top.png", "bottom.png", "left.png", "right.png", mRenderContext);

    initJoystick();

    setMinFps(20);

    err = glGetError();
    assert(err == GL_NO_ERROR);
}

Scene::~Scene()
{

    delete mMonkeyModel;


    SDL_GL_DeleteContext(mContext);
    SDL_DestroyWindow(mWindow);
}

void Scene::setMinFps(double fps)
{
    mMaxSpf = 1 / fps;
}

int Scene::start() {
    std::chrono::high_resolution_clock::time_point previousTime = std::chrono::high_resolution_clock::now();
    while(mRunning)
    {
        std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> delta = std::chrono::duration_cast<std::chrono::duration<double>>(now - previousTime);
        previousTime = now;
        double dt = delta.count();
        if (dt < EPSILON) dt = EPSILON;
        if (dt > mMaxSpf) dt = mMaxSpf;
        double dx = dt / TARGET_SPF;
        update(dt, dx);
        draw();
        double fps = 1 / delta.count();
        fps = fps < MAX_DISPLAY_FPS ? fps : MAX_DISPLAY_FPS;
        mFps = mFps * FPS_SMOOTHNESS + fps * FPS_ONE_FRAME_WEIGHT;
        SDL_GL_SwapWindow(mWindow);
    }
    return 0;
}

void Scene::update(double /* dt */, double dx)
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

    if (state[SDL_SCANCODE_W]) mRenderContext.lightPosition.z += 0.1;
    if (state[SDL_SCANCODE_S]) mRenderContext.lightPosition.z -= 0.1;


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
    mRenderContext.normal = glm::inverseTranspose(glm::mat3(mRenderContext.modelView));

    std::stringstream ss;
    ss << mFps;
    mFpsLabel->setText(ss.str());
    mFpsLabel->update();
}

void Scene::draw()
{
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    mRenderContext.projection = glm::perspective(60.0,
        mRenderContext.screenWidth / (double)mRenderContext.screenHeight, 0.1, 100.0);
    mRenderContext.mvp = mRenderContext.projection * mRenderContext.modelView;

    mMonkeyModel->draw();
    mSkybox->render();

    glDisable(GL_DEPTH_TEST);

    mRenderContext.projection = glm::ortho(0.0f, (float)mRenderContext.screenWidth,
        (float)mRenderContext.screenHeight, 0.0f);
    mRenderContext.mvp = mRenderContext.projection;

    mFpsLabel->draw();
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
