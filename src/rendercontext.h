#ifndef RENDERCONTEXT_H
#define RENDERCONTEXT_H

#include <glm/glm.hpp>

struct RenderContext {
    glm::mat4 projection;
    glm::mat4 view;
    glm::mat4 model;
    glm::mat4 modelView;
    glm::mat4 mvp;
    glm::mat3 normal;
    glm::vec4 lightPosition;
    glm::vec3 diffuseReflectivity;
    glm::vec3 lightSourceIntensity;
};

#endif // RENDERCONTEXT_H
