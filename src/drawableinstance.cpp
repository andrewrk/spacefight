#include "drawableinstance.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

DrawableInstance::DrawableInstance(const Drawable *drawable, RenderContext *renderContext) :
    pos(0),
    scale(1),
    rot(1),
    anchor(0, 0, 0),
    mModelMatrix(1)
{
    init(drawable, renderContext);
}

DrawableInstance::DrawableInstance() :
    DrawableInstance(NULL, NULL)
{
}

void DrawableInstance::init(const Drawable *drawable, RenderContext *renderContext)
{
    mDrawable = drawable;
    mRenderContext = renderContext;
}

void DrawableInstance::update()
{
    mModelMatrix = glm::mat4(1);
    mModelMatrix = glm::translate(mModelMatrix, pos);
    mModelMatrix = glm::scale(mModelMatrix, scale);
    mModelMatrix *= rot;
    mModelMatrix = glm::translate(mModelMatrix, -anchor);
}

void DrawableInstance::draw()
{
    mRenderContext->model = mModelMatrix;
    mRenderContext->calcMvpAndNormal();
    mDrawable->draw(*mRenderContext);
}
