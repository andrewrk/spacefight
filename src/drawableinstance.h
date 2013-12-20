#ifndef MODELINSTANCE_H
#define MODELINSTANCE_H

#include "drawable.h"
#include "rendercontext.h"

#include <glm/glm.hpp>

class DrawableInstance
{
public:
    DrawableInstance(const Drawable *drawable, RenderContext *renderContext);
    DrawableInstance(); // if you use this constructor, call init

    glm::vec3 pos;
    glm::vec3 scale;
    glm::mat4 rot;
    glm::vec3 anchor;

    void init(const Drawable *drawable, RenderContext *renderContext);
    void update();
    void draw();

private:
    const Drawable *mDrawable;

    glm::mat4 mModelMatrix;
    RenderContext *mRenderContext;
};

#endif // MODELINSTANCE_H
