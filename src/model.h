#ifndef MODEL_H
#define MODEL_H

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


#include <GL/glew.h>

class Model
{
public:
    Model(const std::string &filename);

    void draw();

private:


    std::vector<GLuint> mVertexBuffers;
    std::vector<unsigned int> mVertexCounts;

    void importNode(aiNode *node);
};

#endif // MODEL_H
