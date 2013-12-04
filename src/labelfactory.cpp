#include "labelfactory.h"
#include "resourcebundle.h"

LabelFactory::LabelFactory(ResourceBundle *bundle)
{
    textShader = bundle->getShader("text");
    attribTexCoord = textShader->attribLocation("TexCoord");
    attribPosition = textShader->attribLocation("VertexPosition");

    colorUniformId = textShader->uniformId("Color");
    texUniformId = textShader->uniformId("Tex");
    mvpUniformId = textShader->uniformId("MVP");
}

Label *LabelFactory::newLabel()
{
    return new Label(this);
}
