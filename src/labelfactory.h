#ifndef LABELFACTORY_H
#define LABELFACTORY_H

#include "label.h"
#include "shader.h"

class ResourceBundle;

class LabelFactory
{
public:
    LabelFactory(ResourceBundle *bundle);

    Label *newLabel();
private:

    Shader *textShader;
    GLint attribTexCoord;
    GLint attribPosition;

    GLint colorUniformId;
    GLint texUniformId;
    GLint mvpUniformId;

    friend class Label;
};

#endif // LABELFACTORY_H
