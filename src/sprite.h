#ifndef SPRITE_H
#define SPRITE_H

#include "spritesheet.h"

#include "rendercontext.h"

#include <glm/glm.hpp>

class Sprite
{
public:
    Sprite(const SpriteSheet &spritesheet,
           const SpriteSheet::ImageInfo *image,
           const RenderContext &projectionAndView);
    Sprite(const SpriteSheet &spritesheet,
           const std::string &name,
           const RenderContext &projectionAndView);

    void update();
    void draw();

    glm::vec3 pos;
    glm::vec3 scale;
    float rotation;

    int width() const;
    int height() const;

    float scaleWidth() const;
    float scaleHeight() const;
private:
    const SpriteSheet &mSpriteSheet;
    const SpriteSheet::ImageInfo *mImage;

    glm::vec3 mAnchorTrans;

    RenderContext mRenderContext;
    RenderContext mProjectionAndView;
};

#endif // SPRITE_H
