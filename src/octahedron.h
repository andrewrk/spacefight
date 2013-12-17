#ifndef OCTAHEDRON_H
#define OCTAHEDRON_H

#include <vector>

#include <glm/glm.hpp>
#include <GL/glew.h>

namespace Octahedron
{
    void generate(int rowCount, std::vector<glm::vec3> &points, std::vector<GLuint> &indexes);
}

#endif // OCTAHEDRON_H
