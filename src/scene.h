#ifndef SCENE_H
#define SCENE_H


#include <glm/glm.hpp>

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

    struct Asteroid {
        DrawableInstance drawable;
        glm::vec3 vel;
    };

    std::vector<Asteroid> mAsteroids;

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

    glm::vec3 mCameraPosition;
    glm::vec3 mCameraUp; // direction vector
    glm::vec3 mCameraForward; // direction vector
    glm::vec3 mCameraVelocity;

    ResourceBundle mBundle;

    void flushEvents();
    void update(float dt, float dx);
    void draw();
    void initJoystick();
};

#endif // SCENE_H
