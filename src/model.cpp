#include "model.h"
#include <cassert>

Model::Model(const std::string &filename)
{
    Assimp::Importer importer;

    mScene = importer.ReadFile(filename, aiProcess_Triangulate);

    assert(mScene);
}
