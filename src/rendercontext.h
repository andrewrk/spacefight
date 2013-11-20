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
    glm::vec3 lightIntensityAmbient;
    glm::vec3 lightIntensityDiffuse;
    glm::vec3 lightIntensitySpecular;

    glm::vec3 materialReflectivityAmbient;
    glm::vec3 materialReflectivityDiffuse;
    glm::vec3 materialReflectivitySpecular;
    float materialSpecularShininess;

    int screenWidth;
    int screenHeight;
};

#endif // RENDERCONTEXT_H
