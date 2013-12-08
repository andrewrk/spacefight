#include "sprite.h"


#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

Sprite::Sprite(const SpriteSheet &spritesheet, const SpriteSheet::ImageInfo *image, const RenderContext &projectionAndView) :
    mSpriteSheet(spritesheet),
    mImage(image),
    mProjectionAndView(projectionAndView)
{
    mAnchorTrans.x = -image->anchorX;
    mAnchorTrans.y = -image->anchorY;
    mAnchorTrans.z = 0;

    pos.x = 0;
    pos.y = 0;
    pos.z = 0;

    scale.x = 1;
    scale.y = 1;
    scale.z = 1;

    rotation = 0;
}

Sprite::Sprite(const SpriteSheet &spritesheet, const std::string &name, const RenderContext &projectionAndView) :
    Sprite(spritesheet, spritesheet.getImageInfo(name), projectionAndView)
{
}

void Sprite::draw()
{
    mSpriteSheet.draw(mImage, mRenderContext);
}

int Sprite::width() const
{
    return mImage->width;
}

int Sprite::height() const
{
    return mImage->height;
}

float Sprite::scaleWidth() const
{
    return mImage->width * scale.x;
}

float Sprite::scaleHeight() const
{
    return mImage->height * scale.y;
}


void Sprite::update()
{
    mRenderContext.model = glm::translate(glm::scale(glm::rotate(glm::translate(glm::mat4(1.0f), mAnchorTrans), rotation, glm::vec3(0, 0, 1)), scale), pos);
    mRenderContext.modelView = mProjectionAndView.view * mRenderContext.model;
    mRenderContext.mvp = mProjectionAndView.projection * mRenderContext.modelView;
}
