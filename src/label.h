#ifndef LABEL_H
#define LABEL_H

#include <string>

#include <GL/glew.h>

#include <glm/glm.hpp>

#include "rendercontext.h"

class Label
{
public:
    Label(const RenderContext &renderContext);
    ~Label();

    static void init();

    void setFontFace(std::string fontFace);
    void setText(std::string text);
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

    std::string mText;
    std::string mFontFace;

    glm::vec4 mColor;
    const RenderContext &mRenderContext;

};

#endif // LABEL_H
