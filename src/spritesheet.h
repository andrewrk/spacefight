#ifndef SPRITESHEET_H
#define SPRITESHEET_H

#include "shader.h"
#include "rendercontext.h"


#include <string>
#include <map>
#include <vector>

class ResourceBundle;

class SpriteSheet
{
public:
    SpriteSheet();
    ~SpriteSheet();


    struct ImageInfo {
        int x;
        int y;
        int width;
        int height;
        bool r90;
        GLuint vertexArray;
        GLuint vertexBuffer;
        GLuint texCoordBuffer;
    };

    ImageInfo *getImageInfo(const std::string &key);

    void draw(ImageInfo *image, const RenderContext &renderContext);


private:
    GLuint mGlTex = 0;


    Shader *mShader;
    GLint attribTexCoord;
    GLint attribPosition;
    GLint texUniformId;
    GLint mvpUniformId;


    std::map<std::string, ImageInfo> mInfoDict;

    void init(ResourceBundle *bundle);
    void cleanup();
    friend class ResourceBundle;
};

#endif // SPRITESHEET_H
