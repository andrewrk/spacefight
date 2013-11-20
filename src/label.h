#ifndef LABEL_H
#define LABEL_H

#include <string>
#include <vector>

#include <GL/glew.h>

#include <glm/glm.hpp>

#include <pango/pangocairo.h>

#include "rendercontext.h"

class Label
{
public:
    Label(const RenderContext &renderContext);
    ~Label();

    static void init();

    void setFontFace(const std::string &fontFace);
    void setText(const std::string &text);
    void setColor(glm::vec4 color);

    // call this to make your settings changes take effect
    void update();

    void draw();

    int mWidth;
    int mHeight;

private:

    GLuint mVertexArray;
    GLuint mTextureId;
    GLuint mVertexBuffer;
    GLuint mTexCoordBuffer;

    glm::vec4 mColor;
    const RenderContext &mRenderContext;

    std::vector<unsigned char> mImgBuffer;
    cairo_surface_t *mSurface;
    cairo_t *mCairoContext;
    PangoLayout *mLayout;

    void resize();
};

#endif // LABEL_H
