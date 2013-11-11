#include "Input.h"

Input::Input() : mRunning(true)
{
}

Input::~Input()
{
    if(SDL_JoystickOpen(0))
        SDL_JoystickClose(mJoystick);


}

void Input::update()
{
    while(SDL_PollEvent(&mEvent))
    {
        switch(mEvent.type)
        {
            case SDL_KEYDOWN:
                if (mEvent.key.keysym.sym == SDLK_ESCAPE)
                    mRunning = false;
                //if(mEvent.key.)
                break;
            case SDL_QUIT:
                mRunning = false;
                break;
            case SDL_JOYBUTTONDOWN:
                std::cout << "WOAH";
            case SDL_JOYAXISMOTION:
                std::cout << "WOAH";
                if ((mEvent.jaxis.value < -3200) || (mEvent.jaxis.value > 3200))
                {
                    std::cout << "Oh joy" << std::endl;
                }
                break;

            default:
                break;

        }
    }

}

bool Input::initJoystick()
{
    SDL_JoystickEventState(SDL_ENABLE);
    mJoystick = SDL_JoystickOpen(0);

    if(mJoystick == 0)
    {
        std::cout << "Not working: " << SDL_GetError() << std::endl;
        return false;
    }
    return true;
}

bool Input::checkRunning()
{
    return mRunning;
}
