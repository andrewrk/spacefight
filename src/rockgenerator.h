#ifndef ROCKGENERATOR_H
#define ROCKGENERATOR_H

#include "rock.h"

#include "resourcebundle.h"

class RockGenerator
{
public:
    RockGenerator(ResourceBundle *bundle);
    ~RockGenerator();


    void generate(Rock &rock);

private:
    GLuint mTexture = 0;

    Shader *mShader;

    GLint attribPositionIndex;
    GLint attribNormalIndex;
    GLint attribTexCoordIndex;

    std::unique_ptr<Shader::UniformBlock> mLightBlock;
    std::unique_ptr<Shader::UniformBlock> mMaterialBlock;

    GLuint mShaderModelViewMatrix;
    GLuint mShaderNormalMatrix;
    GLuint mShaderProjectionMatrix;
    GLuint mShaderMvp;
    GLuint texUniformId;

    friend class Rock;
};

#endif // ROCKGENERATOR_H
