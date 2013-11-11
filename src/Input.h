#ifndef INPUT_H
#define INPUT_H

#include <SDL2/SDL.h>
#include <iostream>

class Input
{
    public:
        Input();
        virtual ~Input();
        void update();
        //SDL_Keycode getKey();
        bool checkRunning();
        bool initJoystick();
    protected:
    private:
        SDL_Event mEvent;
        bool mRunning;
        SDL_Joystick *mJoystick;



};

#endif // INPUT_H
