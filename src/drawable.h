#ifndef DRAWABLE_H
#define DRAWABLE_H

#include "rendercontext.h"

class Drawable {
public:
    virtual ~Drawable() {}
    virtual void draw(const RenderContext &renderContext) const = 0;
};

#endif // DRAWABLE_H
