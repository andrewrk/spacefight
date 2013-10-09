#include "scene.h"

#include <SDL2/SDL.h>
#include <iostream>


Scene::Scene()
{
}

int Scene::start() {

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Unable to initialize SDL" << std::endl;
        return 1;
    }
    atexit(SDL_Quit);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 4);

    SDL_Window *window = SDL_CreateWindow("Space Fight 3D!",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600,
        SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_OPENGL);

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);

    mainLoop();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    return 0;
}

void Scene::mainLoop() {

    while (m_running) {
        flushEvents();
        std::cout << "heelloo\n";
    }
}

void Scene::flushEvents() {
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE)
                m_running = false;
            break;
        case SDL_QUIT:
            m_running = false;
            break;
        }
    }
}
