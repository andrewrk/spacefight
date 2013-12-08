#ifndef MODELINSTANCE_H
#define MODELINSTANCE_H

#include "model.h"
#include "rendercontext.h"

#include <glm/glm.hpp>

class ModelInstance
{
public:
    ModelInstance(const Model *model, RenderContext *renderContext);
    ModelInstance(); // if you use this constructor, call init

    glm::vec3 pos;
    glm::vec3 scale;
    float roll;
    float pitch;
    float yaw;
    glm::vec3 anchor;

    void init(const Model *model, RenderContext *renderContext);
    void update();
    void draw();

private:
    const Model *mModel;

    glm::mat4 mModelMatrix;
    RenderContext *mRenderContext;
};

#endif // MODELINSTANCE_H
