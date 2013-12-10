#ifndef RENDERCONTEXT_H
#define RENDERCONTEXT_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_inverse.hpp>

struct RenderContext {
    glm::mat4 projection;
    glm::mat4 view;
    glm::mat4 model;
    glm::mat4 modelView;
    glm::mat4 mvp;
    glm::mat3 normal;

    glm::vec3 lightDirection;
    glm::vec3 lightDirectionEye;
    glm::vec3 lightIntensityAmbient;
    glm::vec3 lightIntensityDiffuse;
    glm::vec3 lightIntensitySpecular;

    glm::vec3 materialReflectivityAmbient;
    glm::vec3 materialReflectivityDiffuse;
    glm::vec3 materialReflectivitySpecular;
    float materialSpecularShininess;

    inline void calcMvpAndNormal() {
        calcMvp();
        normal = glm::inverseTranspose(glm::mat3(modelView));
        lightDirectionEye = glm::normalize(normal * -lightDirection);
    }

    inline void calcMvp() {
        modelView = view * model;
        mvp = projection * modelView;
    }
};

#endif // RENDERCONTEXT_H
