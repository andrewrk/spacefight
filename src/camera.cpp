#include "camera.h"

camera::camera(glm::vec3 position, glm::vec3 target, glm::vec3 upDirection, float movementSpeed, float lookSpeed)
    : mPosition(position), mTarget(target), upDirection(upDirection), mMovementSpeed(movementSpeed), mLookSpeed(lookSpeed)
{

}

camera::~camera()
{

}

void camera::updateModelview(glm::mat4 &modelview)
{

}

void camera::move(Input &input)
{
    if(input.getKey(SDL_SCANCODE_A) == true)
        std::cout << "YAY" << std::endl;
}

/*void camera::lookAt(glm::vec3)
{

}*/
