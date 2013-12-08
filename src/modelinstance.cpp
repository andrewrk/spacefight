#include "modelinstance.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

ModelInstance::ModelInstance(const Model *model, RenderContext *renderContext) :
    pos(0),
    scale(1),
    roll(0),
    pitch(0),
    yaw(0),
    anchor(0, 0, 0),
    mModelMatrix(1)
{
    init(model, renderContext);
}

ModelInstance::ModelInstance() :
    ModelInstance(NULL, NULL)
{
}

void ModelInstance::init(const Model *model, RenderContext *renderContext)
{
    mModel = model;
    mRenderContext = renderContext;
}

void ModelInstance::update()
{
    mModelMatrix = glm::mat4(1);
    mModelMatrix = glm::translate(mModelMatrix, pos);
    mModelMatrix = glm::scale(mModelMatrix, scale);
    mModelMatrix = glm::rotate(mModelMatrix, roll, glm::vec3(1, 0, 0));
    mModelMatrix = glm::rotate(mModelMatrix, pitch, glm::vec3(0, 1, 0));
    mModelMatrix = glm::rotate(mModelMatrix, yaw, glm::vec3(0, 0, 1));
    mModelMatrix = glm::translate(mModelMatrix, -anchor);
}

void ModelInstance::draw()
{
    mRenderContext->model = mModelMatrix;
    mRenderContext->calcMvpAndNormal();
    mModel->draw(*mRenderContext);
}
