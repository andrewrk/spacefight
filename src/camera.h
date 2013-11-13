#ifndef CAMERA_H
#define CAMERA_H

//GLM
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Input.h"


class camera
{
    public:
        camera(glm::vec3 position, glm::vec3 target, glm::vec3 upDirection, float movementSpeed, float lookSpeed);
        virtual ~camera();
        void updateModelview(glm::mat4 &modelview);
        void move(Input &input);

    protected:
    private:
        float mMovementSpeed;
        float mLookSpeed;
        glm::vec3 mTarget;
        glm::vec3 mPosition;
        glm::vec3 upDirection;

};

#endif // CAMERA_H
