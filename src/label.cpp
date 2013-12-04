#include "label.h"
#include "labelfactory.h"

#include <iostream>


Label::Label(LabelFactory *factory) :
    mWidth(0),
    mHeight(0),
    mSurfaceWidth(0),
    mSurfaceHeight(0),
    mFactory(factory)
{
    glActiveTexture(GL_TEXTURE0); // what the fuck does this do?
    glGenTextures(1, &mTextureId);
    glBindTexture(GL_TEXTURE_2D, mTextureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    glGenVertexArrays(1, &mVertexArray);
    glBindVertexArray(mVertexArray);

    glGenBuffers(1, &mVertexBuffer);
    glGenBuffers(1, &mTexCoordBuffer);

    if (mFactory->attribPosition == -1) {
        std::cerr << "warning: text shader discarding vertex buffer data\n";
    } else {
        // send dummy data - real deal happens at update()
        GLfloat vertexes[4][3] = {
            {0, 0, 0},
            {0, 0, 0},
            {0, 0, 0},
            {0, 0, 0},
        };
        glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, 4 * 3 * sizeof(GLfloat), vertexes, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(mFactory->attribPosition);
        glVertexAttribPointer(mFactory->attribPosition, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    }

    if (mFactory->attribTexCoord == -1) {
        std::cerr << "warning: text shader discarding tex coord data\n";
    } else {
        GLfloat coords[4][2] = {
            {0, 0},
            {1, 0},
            {0, 1},
            {1, 1},
        };
        glBindBuffer(GL_ARRAY_BUFFER, mTexCoordBuffer);
        glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(GLfloat), coords, GL_STATIC_DRAW);
        glEnableVertexAttribArray(mFactory->attribTexCoord);
        glVertexAttribPointer(mFactory->attribTexCoord, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    }

    GLenum err = glGetError();
    assert(err == GL_NO_ERROR);

    int stride = cairo_format_stride_for_width(CAIRO_FORMAT_ARGB32, mSurfaceWidth);
    mSurface = cairo_image_surface_create_for_data(&mImgBuffer[0], CAIRO_FORMAT_ARGB32, mSurfaceWidth, mSurfaceHeight, stride);
    mCairoContext = cairo_create(mSurface);
    mLayout = pango_cairo_create_layout(mCairoContext);
    cairo_set_source_rgba(mCairoContext, 1, 1, 1, 1);

    mColor = glm::vec4(0, 0, 0, 1);
    setFontFace("Sans Bold 18");
}

Label::~Label()
{
    glDeleteBuffers(1, &mVertexBuffer);
    glDeleteBuffers(1, &mTexCoordBuffer);
    glDeleteVertexArrays(1, &mVertexArray);
    glDeleteTextures(1, &mTextureId);

    g_object_unref(mLayout);
    cairo_surface_destroy(mSurface);
    cairo_destroy(mCairoContext);
}

void Label::maybeResize()
{
    pango_layout_get_size(mLayout, &mWidth, &mHeight);
    mWidth /= PANGO_SCALE;
    mHeight /= PANGO_SCALE;

    unsigned int imgBufSize = 4 * mWidth * mHeight;

    if (imgBufSize <= mImgBuffer.size())
        return;

    mImgBuffer.resize(imgBufSize);
    if (mWidth > mSurfaceWidth) mSurfaceWidth = mWidth;
    if (mHeight > mSurfaceHeight) mSurfaceHeight = mHeight;
    int stride = cairo_format_stride_for_width(CAIRO_FORMAT_ARGB32, mSurfaceWidth);

    cairo_surface_destroy(mSurface);
    mSurface = cairo_image_surface_create_for_data(&mImgBuffer[0], CAIRO_FORMAT_ARGB32, mSurfaceWidth, mSurfaceHeight, stride);

    cairo_destroy(mCairoContext);
    mCairoContext = cairo_create(mSurface);

    cairo_set_source_rgba(mCairoContext, 1, 1, 1, 1);

    pango_cairo_update_layout(mCairoContext, mLayout);

    glBindVertexArray(mVertexArray);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
    GLfloat vertexes[4][3] = {
        {0, 0, 0},
        {(float)mSurfaceWidth, 0, 0},
        {0, (float)mSurfaceHeight, 0},
        {(float)mSurfaceWidth, (float)mSurfaceHeight, 0},
    };
    glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * 4 * sizeof(GLfloat), vertexes);

    GLenum err = glGetError();
    assert(err == GL_NO_ERROR);
}

void Label::setFontFace(const std::string &fontFace)
{
    PangoFontDescription *desc = pango_font_description_from_string(fontFace.c_str());
    pango_layout_set_font_description(mLayout, desc);
    pango_font_description_free(desc);
    maybeResize();
}

void Label::setText(const std::string &text)
{
    pango_layout_set_text(mLayout, text.data(), text.size());
    maybeResize();
}

void Label::setColor(glm::vec4 color)
{
    mColor = color;
}

void Label::update()
{
    std::fill(mImgBuffer.begin(), mImgBuffer.end(), 0);
    pango_cairo_show_layout(mCairoContext, mLayout);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mTextureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mSurfaceWidth, mSurfaceHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, &mImgBuffer[0]);

    GLenum err = glGetError();
    assert(err == GL_NO_ERROR);
}

void Label::draw(const RenderContext &renderContext)
{
    mFactory->textShader->bind();

    mFactory->textShader->setUniform(mFactory->colorUniformId, mColor);
    mFactory->textShader->setUniform(mFactory->texUniformId, 0);
    mFactory->textShader->setUniform(mFactory->mvpUniformId, renderContext.mvp);

    glBindVertexArray(mVertexArray);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mTextureId);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    mFactory->textShader->unbind();
}
