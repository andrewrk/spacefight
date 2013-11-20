#include "label.h"
#include <pango/pangocairo.h>
#include "shadermanager.h"

#include <iostream>

static Shader *textShader = NULL;
static GLint attribTexCoord;
static GLint attribPosition;

static GLint colorUniformId;
static GLint texUniformId;
static GLint mvpUniformId;

void Label::init()
{
    textShader = ShaderManager::getShader("text");
    attribTexCoord = textShader->attribLocation("TexCoord");
    attribPosition = textShader->attribLocation("VertexPosition");

    colorUniformId = textShader->uniformId("Color");
    texUniformId = textShader->uniformId("Tex");
    mvpUniformId = textShader->uniformId("MVP");

}

Label::Label(const RenderContext &renderContext)
    : mRenderContext(renderContext)
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

    if (attribPosition == -1) {
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
        glEnableVertexAttribArray(attribPosition);
        glVertexAttribPointer(attribPosition, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    }

    if (attribTexCoord == -1) {
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
        glEnableVertexAttribArray(attribTexCoord);
        glVertexAttribPointer(attribTexCoord, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    }

    GLenum err = glGetError();
    assert(err == GL_NO_ERROR);

    mColor = glm::vec4(0, 0, 0, 1);
    mFontFace = "Sans Bold 18";
}

Label::~Label()
{
    glDeleteBuffers(1, &mVertexBuffer);
    glDeleteBuffers(1, &mTexCoordBuffer);
    glDeleteVertexArrays(1, &mVertexArray);
    glDeleteTextures(1, &mTextureId);
}

void Label::setFontFace(std::string fontFace)
{
    mFontFace = fontFace;
}

void Label::setText(std::string text)
{
    mText = text;
}

void Label::setColor(glm::vec4 color)
{
    mColor = color;
}

void Label::update()
{
    cairo_surface_t *temp_surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, 0, 0);
    cairo_t *context = cairo_create (temp_surface);
    cairo_surface_destroy (temp_surface);

    /* Create a PangoLayout, set the font and text */
    PangoLayout *layout = pango_cairo_create_layout (context);
    pango_layout_set_text (layout, mText.c_str(), -1);

    /* Load the font */
    PangoFontDescription *desc = pango_font_description_from_string (mFontFace.c_str());
    pango_layout_set_font_description (layout, desc);
    pango_font_description_free (desc);

    /* Get text dimensions and create a context to render to */
    pango_layout_get_size(layout, &mWidth, &mHeight);
    mWidth /= PANGO_SCALE;
    mHeight /= PANGO_SCALE;
    unsigned char *buffer = (unsigned char*) calloc(4 * mWidth * mHeight, 1);
    cairo_surface_t *surface = cairo_image_surface_create_for_data(buffer, CAIRO_FORMAT_ARGB32, mWidth, mHeight, 4 * mWidth);
    cairo_t *render_context = cairo_create(surface);


    /* Render */
    cairo_set_source_rgba (render_context, 1, 1, 1, 1);
    pango_cairo_show_layout (render_context, layout);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mTextureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mWidth, mHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, buffer);
    glBindVertexArray(mVertexArray);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
    GLfloat vertexes[4][3] = {
        {0, 0, 0},
        {(float)mWidth, 0, 0},
        {0, (float)mHeight, 0},
        {(float)mWidth, (float)mHeight, 0},
    };
    glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * 4 * sizeof(GLfloat), vertexes);

    /* Clean up */
    free (buffer);
    g_object_unref (layout);
    cairo_destroy (context);
    cairo_destroy (render_context);
    cairo_surface_destroy (surface);

    GLenum err = glGetError();
    assert(err == GL_NO_ERROR);
}

void Label::draw()
{
    textShader->bind();

    textShader->setUniform(colorUniformId, mColor);
    textShader->setUniform(texUniformId, 0);
    textShader->setUniform(mvpUniformId, mRenderContext.mvp);

    glBindVertexArray(mVertexArray);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mTextureId);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    textShader->unbind();
}
