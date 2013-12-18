#ifndef SCENE_H
#define SCENE_H


#include <glm/glm.hpp>

#include "laserbeam.h"
#include "shader.h"
#include "drawableinstance.h"
#include "rendercontext.h"
#include "rock.h"

#include "resourcebundle.h"

#include <SDL2/SDL.h>
#include <GL/glew.h>

#include "label.h"
#include "labelfactory.h"
#include "rockgenerator.h"

#include "spacebox.h"

#include "spritesheet.h"
#include "sprite.h"

#include "boundarysphere.h"

class Scene
{
public:
    Scene();
    ~Scene();
    int start();
    void setMinFps(float fps);
private:
    bool mRunning = true;
    SDL_GLContext mContext;
    SDL_Window *mWindow;

    RenderContext m3DRenderContext;
    RenderContext m2DRenderContext;
    RenderContext mSkyBoxRenderContext;

    int mScreenWidth;
    int mScreenHeight;

    std::vector<Rock> mRockTypes;

    class PhysicsSphere {
    public:
        PhysicsSphere(float mass, float collisionDamping) :
            mass(mass),
            collisionDamping(collisionDamping) {}
        float mass = 1.0f;
        float collisionDamping = 1.0f;

        virtual glm::vec3 vel() const = 0;
        virtual void setVel(const glm::vec3 &value) = 0;

        virtual glm::vec3 pos() const = 0;
        virtual void setPos(const glm::vec3 &value) = 0;

        virtual float radius() const = 0;
    };

    class Asteroid : public PhysicsSphere {
    public:
        Asteroid() : PhysicsSphere(1.0f, 1.0f) {}
        DrawableInstance drawable;
        glm::vec3 mVel;

        glm::vec3 vel() const override {
            return mVel;
        }

        void setVel(const glm::vec3 &value) override {
            mVel = value;
        }

        glm::vec3 pos() const override {
            return drawable.pos;
        }

        void setPos(const glm::vec3 &value) override {
            drawable.pos = value;
        }

        float radius() const override {
            return drawable.scale[0];
        }
    };

    class ShipPhysicsObject : public PhysicsSphere {
    public:
        ShipPhysicsObject(float radius) :
            PhysicsSphere(0.1f, 1.0f),
            mVel(0),
            mPos(0),
            mRadius(radius) {}
        glm::vec3 mVel;
        glm::vec3 mPos;
        float mRadius;

        glm::vec3 vel() const override {
            return mVel;
        }

        void setVel(const glm::vec3 &value) override {
            mVel = value;
        }

        glm::vec3 pos() const override {
            return mPos;
        }

        void setPos(const glm::vec3 &value) override {
            mPos = value;
        }

        float radius() const override {
            return mRadius;
        }

    };

    std::vector<Asteroid> mAsteroids;


    struct Bullet {
        glm::vec3 pos;
        glm::vec3 vel;
    };

    std::vector<Bullet> mBullets;
    float timeUntilFire = 0;
    float maxTimeUntilFire = 0.2;

    bool cullOn = true;
    bool solidOn = true;

    SpaceBox *mSpaceBox;

    LabelFactory *mLabelFactory;

    RockGenerator *mRockGenerator;

    float mMaxSpf;
    float mFps;
    Label *mFpsLabel;

    SpriteSheet mCockpitSpritesheet;
    Sprite *mVelDisplayOutline;
    Sprite *mVelDisplayArrow;
    Sprite *mCrossHair;
    Sprite *mForwardVelArrow;

    float mArenaRadius;
    BoundarySphere *mBoundarySphere;

    std::vector<SDL_Joystick*> mJoysticks;

    glm::vec3 mCameraPos;
    glm::vec3 mPlayerUp; // direction vector
    glm::vec3 mPlayerForward; // direction vector
    ShipPhysicsObject *mPlayerShip;

    LaserBeam *mLaserBeam;

    ResourceBundle mBundle;

    void flushEvents();
    void update(float dt, float dx);
    void draw();
    void initJoystick();
    void calcCameraPos();
    void maybeBounceObjOffBoundary(PhysicsSphere *obj);
    void fireLaser(const glm::vec3 &pos, const glm::vec3 &vel);
};

#endif // SCENE_H
