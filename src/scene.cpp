#include "scene.h"


#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <iostream>

#include <limits>
#include <cstdint>
#include <ctime>
#include <sstream>
#include <chrono>

#include <SDL2/SDL_joystick.h>


static const float CAMERA_ROTATION_SPEED = 0.0262f;

static const float EPSILON = 0.0000000001;
static const float MAX_DISPLAY_FPS = 90000;
static const float TARGET_FPS = 60.0;
static const float TARGET_SPF = 1.0 / TARGET_FPS;
static const float FPS_SMOOTHNESS = 0.9;
static const float FPS_ONE_FRAME_WEIGHT = 1.0 - FPS_SMOOTHNESS;

static const float ENGINE_THRUST = 0.001;

static const float FIELD_OF_VIEW = 1.047;

static const float LASER_SPEED = 1;

static const float PI = 3.14159265358979;

static float randFloat() {
    return rand() / (float) RAND_MAX;
}

static glm::vec3 randDir() {
    return glm::normalize(glm::vec3(randFloat(), randFloat(), randFloat()));
}

Scene::Scene() :
    mBundle("assets.bundle")
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

    mScreenWidth = 1366;
    mScreenHeight = 768;

    mWindow = SDL_CreateWindow("Space Fight 3D!",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        mScreenWidth, mScreenHeight, SDL_WINDOW_OPENGL|SDL_WINDOW_FULLSCREEN_DESKTOP);
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


    glEnable(GL_CULL_FACE);

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glDepthFunc(GL_LESS);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    mLaserBeam = new LaserBeam(&mBundle);


    mArenaRadius = 100.0f;
    mBoundarySphere = new BoundarySphere(&mBundle);
    mBoundarySphere->generate(20);

    mPlayerShip = new ShipPhysicsObject(3.0f);
    mPlayerForward = glm::vec3(0, -1.0, 0);
    mPlayerUp = glm::vec3(0, 0, 1);
    calcCameraPos();

    m3DRenderContext.projection = glm::perspective(FIELD_OF_VIEW,
        mScreenWidth / (float)mScreenHeight, 0.1f, mArenaRadius * 2 + 10.0f);
    m3DRenderContext.model = glm::mat4(1.0);
    m3DRenderContext.lightDirection = glm::vec3(0, 0, -1);
    m3DRenderContext.lightIntensityAmbient = glm::vec3(0.2, 0.2, 0.2);
    m3DRenderContext.lightIntensityDiffuse = glm::vec3(0.5, 0.5, 0.5);
    m3DRenderContext.lightIntensitySpecular = glm::vec3(0.8, 0.8, 0.8);
    m3DRenderContext.materialReflectivityAmbient = glm::vec3(0.7, 0.7, 0.7);
    m3DRenderContext.materialReflectivityDiffuse = glm::vec3(0.9, 0.9, 0.9);
    m3DRenderContext.materialReflectivitySpecular = glm::vec3(0.1, 0.1, 0.1);
    m3DRenderContext.materialSpecularShininess = 50.0f;

    mSkyBoxRenderContext = m3DRenderContext;
    mSkyBoxRenderContext.model = glm::scale(glm::mat4(1.0f), glm::vec3(10.0f, 10.0f, 10.0f));


    mSpaceBox = new SpaceBox(&mBundle);
    mSpaceBox->generate();

    mLabelFactory = new LabelFactory(&mBundle);

    mFpsLabel = mLabelFactory->newLabel();
    mFpsLabel->setColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    mFpsLabel->setFontFace("Sans 12");

    m2DRenderContext.projection = glm::ortho(0.0f, (float)mScreenWidth,
        (float)mScreenHeight, 0.0f);
    m2DRenderContext.view = glm::mat4(1.0);
    m2DRenderContext.calcMvp();

    mBundle.getSpriteSheet("cockpit", mCockpitSpritesheet);
    mVelDisplayOutline = new Sprite(mCockpitSpritesheet, "radarCircle", m2DRenderContext);
    mVelDisplayOutline->pos.x = mScreenWidth - mVelDisplayOutline->scaleWidth() / 2;
    mVelDisplayOutline->pos.y = mScreenHeight - mVelDisplayOutline->scaleHeight() / 2;
    mVelDisplayOutline->update();

    mVelDisplayArrow = new Sprite(mCockpitSpritesheet, "arrow", m2DRenderContext);
    mVelDisplayArrow->pos = mVelDisplayOutline->pos;
    mVelDisplayArrow->update();

    const int margin = 20;
    mForwardVelArrow = new Sprite(mCockpitSpritesheet, "arrow", m2DRenderContext);
    mForwardVelArrow->pos = mVelDisplayOutline->pos - glm::vec3(mVelDisplayOutline->width() / 2 + margin, 0, 0);
    mForwardVelArrow->rotation = PI / 2;
    mForwardVelArrow->update();

    mCrossHair = new Sprite(mCockpitSpritesheet, "crosshair", m2DRenderContext);
    mCrossHair->pos.x = mScreenWidth / 2.0f;
    mCrossHair->pos.y = mScreenHeight / 2.0f;
    mCrossHair->update();

    mRockGenerator = new RockGenerator(&mBundle);

    mRockTypes.resize(50);
    for (unsigned int i = 0; i < mRockTypes.size(); i += 1) {
        Rock *rockType = &mRockTypes[i];
        mRockGenerator->generate(*rockType);
    }
    mAsteroids.resize(4);
    const float objMaxRadius = 70.0f;
    const float objMinRadius = 30.0f;
    for (unsigned int i = 0; i < mAsteroids.size(); i += 1) {
        Asteroid *asteroid = &mAsteroids[i];
        asteroid->setVel(randDir() * 0.1f);
        asteroid->drawable.init(&mRockTypes[rand() % mRockTypes.size()], &m3DRenderContext);
        float radius = objMinRadius + objMaxRadius * randFloat();
        asteroid->drawable.pos = randDir() * radius;
        asteroid->drawable.scale = glm::vec3(15.0f, 15.0f, 15.0f);
        asteroid->drawable.update();
    }

    initJoystick();

    setMinFps(20);

    err = glGetError();
    assert(err == GL_NO_ERROR);
}

Scene::~Scene()
{
    delete mLabelFactory;

    SDL_GL_DeleteContext(mContext);
    SDL_DestroyWindow(mWindow);
}

void Scene::calcCameraPos()
{
    mCameraPos = mPlayerShip->pos() + mPlayerForward * mPlayerShip->radius();
}

void Scene::maybeBounceObjOffBoundary(Scene::PhysicsSphere *obj)
{
    if (glm::length(obj->pos()) + obj->radius() >= mArenaRadius) {
        glm::vec3 normal = glm::normalize(obj->pos());
        glm::vec3 normVel = glm::normalize(obj->vel());
        float velMagnitude = glm::length(obj->vel());
        obj->setVel(glm::reflect(normVel, -normal) * velMagnitude);
        obj->setPos(normal * (mArenaRadius - obj->radius() - 1));
    }

}

void Scene::fireLaser(const glm::vec3 &pos, const glm::vec3 &vel)
{
    mBullets.resize(mBullets.size() + 1);
    Bullet *bullet = &mBullets[mBullets.size() - 1];
    bullet->drawable.init(mLaserBeam, &m3DRenderContext);
    bullet->drawable.scale = glm::vec3(2);
    bullet->drawable.anchor = glm::vec3(0, 0, 1);
    bullet->setPos(pos);
    bullet->mVel = vel;

    bullet->correctRotation();
}

void Scene::setMinFps(float fps)
{
    mMaxSpf = 1 / fps;
}

int Scene::start() {
    std::chrono::high_resolution_clock::time_point previousTime = std::chrono::high_resolution_clock::now();
    while(mRunning)
    {
        std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> delta = std::chrono::duration_cast<std::chrono::duration<float>>(now - previousTime);
        previousTime = now;
        float dt = delta.count();
        if (dt < EPSILON) dt = EPSILON;
        if (dt > mMaxSpf) dt = mMaxSpf;
        float dx = dt / TARGET_SPF;
        update(dt, dx);
        draw();
        float fps = 1 / delta.count();
        fps = fps < MAX_DISPLAY_FPS ? fps : MAX_DISPLAY_FPS;
        mFps = mFps * FPS_SMOOTHNESS + fps * FPS_ONE_FRAME_WEIGHT;
        SDL_GL_SwapWindow(mWindow);
    }
    return 0;
}

static float clamp(float min, float value, float max)
{
    return value < min ? min : (value > max ? max : value);
}

static glm::mat4 lookWithoutPosition(const glm::vec3 &eye, const glm::vec3 &center, const glm::vec3 &up)
{
    glm::vec3 f = glm::normalize(center - eye);
    glm::vec3 u = glm::normalize(up);
    glm::vec3 s = glm::normalize(glm::cross(f, u));
    u = glm::cross(s, f);

    glm::mat4 Result(1);
    Result[0][0] = s.x;
    Result[1][0] = s.y;
    Result[2][0] = s.z;

    Result[0][1] = u.x;
    Result[1][1] = u.y;
    Result[2][1] = u.z;

    Result[0][2] =-f.x;
    Result[1][2] =-f.y;
    Result[2][2] =-f.z;

    Result[3][0] = 0;
    Result[3][1] = 0;
    Result[3][2] = 1;
    return Result;
}

static glm::mat3 changeBasisMatrix(const glm::vec3 &forward, const glm::vec3 &up)
{
    glm::vec3 f = glm::normalize(forward);
    glm::vec3 u = glm::normalize(up);
    glm::vec3 s = glm::normalize(glm::cross(f, u));

    glm::mat3 Result(1);
    Result[0][0] = s.x;
    Result[1][0] = s.y;
    Result[2][0] = s.z;

    Result[0][1] = u.x;
    Result[1][1] = u.y;
    Result[2][1] = u.z;

    Result[0][2] = f.x;
    Result[1][2] = f.y;
    Result[2][2] = f.z;
    return Result;
}

static float velToScale(float vel) {
    return vel / 0.1;
}

void Scene::update(float dt, float dx)
{
    flushEvents();

    float joyX = 0.0;
    float joyY = 0.0;
    float joyZ = 0.0;
    float joyEngine = 0.0;
    char joyFire = 0;
    for (unsigned int i = 0; i < mJoysticks.size(); i += 1) {
        SDL_Joystick *joystick = mJoysticks[i];

        Sint16 val = SDL_JoystickGetAxis(joystick, 0);
        joyX += (float) val / (float) INT16_MAX;

        val = SDL_JoystickGetAxis(joystick, 1);
        joyY += (float) val / (float) INT16_MAX;

        val = SDL_JoystickGetAxis(joystick, 2);
        joyZ += (float) val / (float) INT16_MAX;

        Uint8 hatVal = SDL_JoystickGetHat(joystick, 0);

        if ((hatVal&SDL_HAT_UP) ||
            (hatVal&SDL_HAT_LEFTUP) ||
            (hatVal&SDL_HAT_RIGHTUP))
        {
            joyEngine += 1.0f;
        }
        if ((hatVal&SDL_HAT_DOWN) ||
            (hatVal&SDL_HAT_LEFTDOWN) ||
            (hatVal&SDL_HAT_RIGHTDOWN))
        {
            joyEngine -= 1.0f;
        }
    }
    const Uint8 *state = SDL_GetKeyboardState(NULL);
    if (state[SDL_SCANCODE_LEFT]) joyX -= 1.0;
    if (state[SDL_SCANCODE_RIGHT]) joyX += 1.0;
    if (state[SDL_SCANCODE_UP]) joyY += 1.0;
    if (state[SDL_SCANCODE_DOWN]) joyY -= 1.0;
    if (state[SDL_SCANCODE_A]) joyZ += 1.0;
    if (state[SDL_SCANCODE_D]) joyZ -= 1.0;
    if (state[SDL_SCANCODE_W]) joyEngine += 1.0;
    if (state[SDL_SCANCODE_S]) joyEngine -= 1.0;
    if (state[SDL_SCANCODE_SPACE]) joyFire = 1;

    timeUntilFire -= dt;
    if (timeUntilFire <= 0) {
        if (joyFire) {
            timeUntilFire = maxTimeUntilFire;
            fireLaser(mPlayerShip->mPos, mPlayerShip->mVel + mPlayerForward * LASER_SPEED);
        }
    }


    joyX = clamp(-1.0f, joyX, 1.0f);
    joyY = clamp(-1.0f, joyY, 1.0f);
    joyZ = clamp(-1.0f, joyZ, 1.0f);
    joyEngine = clamp(-1.0f, joyEngine, 1.0f);

    // x^3 gives us a nice joystick response curve
    float deltaAngle = -joyX * joyX * joyX * CAMERA_ROTATION_SPEED * dx;
    mPlayerForward = glm::rotate(mPlayerForward, deltaAngle, mPlayerUp);
    glm::vec3 leftVector = glm::cross(mPlayerForward, mPlayerUp);

    deltaAngle = joyY * joyY * joyY * CAMERA_ROTATION_SPEED * dx;
    mPlayerForward = glm::rotate(mPlayerForward, deltaAngle, leftVector);
    mPlayerUp = glm::cross(leftVector, mPlayerForward);

    deltaAngle = joyZ * joyZ * joyZ * CAMERA_ROTATION_SPEED * dx;
    mPlayerUp = glm::rotate(mPlayerUp, deltaAngle, mPlayerForward);

    mPlayerShip->mPos += mPlayerShip->mVel * dx;
    mPlayerShip->mVel += mPlayerForward * (ENGINE_THRUST * joyEngine * dx);

    // player collide with boundary
    maybeBounceObjOffBoundary(mPlayerShip);
    calcCameraPos();


    m3DRenderContext.view = glm::lookAt(mCameraPos, mCameraPos + mPlayerForward, mPlayerUp);
    m3DRenderContext.calcMvpAndNormal();

    for (unsigned int i = 0; i < mBullets.size(); i += 1) {
        Bullet *bullet = &mBullets[i];
        bullet->setPos(bullet->pos() + bullet->mVel * dx);

        maybeBounceObjOffBoundary(bullet);
        bullet->correctRotation();
        bullet->drawable.update();
    }

    for (unsigned int i = 0; i < mAsteroids.size(); i += 1) {
        Asteroid *asteroid = &mAsteroids[i];
        asteroid->drawable.pos += asteroid->mVel * dx;

        // check for collisions
        for (unsigned int j = i + 1; j < mAsteroids.size(); j += 1) {
            Asteroid *other = &mAsteroids[j];
            glm::vec3 deltaVector = other->drawable.pos - asteroid->drawable.pos;
            float distanceApart = glm::length(deltaVector);
            float summedRadii = asteroid->radius() + other->radius();
            if (distanceApart < summedRadii) {
                // calculate normal
                glm::vec3 normal = glm::normalize(deltaVector);
                // calculate relative velocity
                glm::vec3 rv = other->mVel - asteroid->mVel;
                // relative velocity in terms of the normal direction
                float velAlongNormal = glm::dot(rv, normal);
                // do not resolve if velocities are separating
                if (velAlongNormal <= 0) {
                    // calculate restitution
                    float e = MIN(asteroid->collisionDamping, other->collisionDamping);
                    // calculate impulse scalar
                    float j = -(1 + e) * velAlongNormal;
                    j /= 1 / asteroid->mass + 1 / other->mass;
                    // apply impulse
                    glm::vec3 impulse = normal * j;
                    asteroid->mVel -= impulse / asteroid->mass;
                    other->mVel += impulse / other->mass;
                }

                break;
            }
        }

        // check for hitting boundary
        maybeBounceObjOffBoundary(asteroid);

        asteroid->drawable.update();
    }

    mSkyBoxRenderContext.view = lookWithoutPosition(mCameraPos, mCameraPos + mPlayerForward, mPlayerUp);
    mSkyBoxRenderContext.calcMvpAndNormal();

    // calculate the magnitude and direction of velocity in the direction other than forward or backward
    // convert velocity vector from absolute coordinates into coordinate system relative to ship orientation
    // then we can simply take 2 of the 3 components.
    glm::mat3 theMatrix = changeBasisMatrix(mPlayerForward, mPlayerUp);
    glm::vec3 relVel = theMatrix * mPlayerShip->mVel;
    glm::vec2 relVel2D(relVel[0], relVel[1]);
    float forwardVel = -relVel[2];
    float relVelMagnitude = glm::length(relVel2D);
    if (relVelMagnitude > 0) {
        mVelDisplayArrow->rotation = atan2(-relVel2D[1], relVel2D[0]);
        mVelDisplayArrow->scale = glm::vec3(velToScale(relVelMagnitude));
    } else {
        mVelDisplayArrow->scale = glm::vec3(0);
    }
    mVelDisplayArrow->update();

    mForwardVelArrow->scale = glm::vec3(velToScale(forwardVel));
    mForwardVelArrow->update();

    std::stringstream ss;
    ss << mFps;
    mFpsLabel->setText(ss.str());
    mFpsLabel->update();

}

void Scene::draw()
{
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    mSpaceBox->draw(mSkyBoxRenderContext);
    glDepthMask(GL_TRUE);

    m3DRenderContext.model = glm::scale(glm::mat4(1), glm::vec3(mArenaRadius));
    m3DRenderContext.calcMvpAndNormal();
    mBoundarySphere->draw(m3DRenderContext);



    for (unsigned int i = 0; i < mAsteroids.size(); i += 1) {
        Asteroid *asteroid = &mAsteroids[i];
        asteroid->drawable.draw();
    }

    for (unsigned int i = 0; i < mBullets.size(); i += 1) {
        Bullet *bullet = &mBullets[i];
        bullet->drawable.draw();
    }


    glDisable(GL_DEPTH_TEST);

    mCrossHair->draw();
    mVelDisplayOutline->draw();
    mVelDisplayArrow->draw();
    mForwardVelArrow->draw();

    m2DRenderContext.model = glm::translate(glm::mat4(1.0), glm::vec3(0, mScreenHeight - mFpsLabel->mHeight, 0));
    m2DRenderContext.calcMvp();
    mFpsLabel->draw(m2DRenderContext);
}

void Scene::flushEvents() {
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_KEYDOWN:
            switch (event.key.keysym.scancode) {
            case SDL_SCANCODE_ESCAPE:
                mRunning = false;
                break;
            case SDL_SCANCODE_C:
                cullOn = !cullOn;
                if (cullOn) {
                    glEnable(GL_CULL_FACE);
                } else {
                    glDisable(GL_CULL_FACE);
                }
                break;
            case SDL_SCANCODE_P:
                solidOn = !solidOn;
                if (solidOn) {
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                } else {
                    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                }
                break;
            default:
                break;
            }
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


void Scene::Bullet::correctRotation()
{
    glm::vec3 forward = glm::normalize(mVel);
    drawable.pitch = atan2(forward.x, forward.z);
    drawable.roll = asin(-forward.y);
}
