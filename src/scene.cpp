#include "scene.h"

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

static const GLfloat vertex_buffer_data[] = {
    -1.0f, -1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
};

Scene::Scene()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Unable to initialize SDL\n";
        exit(1);
    }
    atexit(SDL_Quit);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    int width = 1366;
    int height = 768;
    mWindow = SDL_CreateWindow("Space Fight 3D!",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height,
        SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_OPENGL);
    if (!mWindow) {
        std::cerr << "Unable to create window\n";
        exit(1);
    }

    mContext = SDL_GL_CreateContext(mWindow);

    glewExperimental = GL_TRUE;
    GLenum status = glewInit();
    if (status != GLEW_OK) {
        std::cerr << "GLEW error:" << glewGetErrorString(status) << "\n";
        exit(1);
    }

    // set buffer swap with monitor's vertical refresh rate
    SDL_GL_SetSwapInterval(1);

    glGenVertexArrays(1, &mVertexArrayId);
    glBindVertexArray(mVertexArrayId);

    glGenBuffers(1, &mVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data), vertex_buffer_data, GL_STATIC_DRAW);

    mShader = new Shader("src/shaders/basic.vert", "src/shaders/basic.frag");

    mProjection = glm::perspective(60.0, width / (double)height, 0.1, 100.0);
    mView = glm::lookAt(
                glm::vec3(4.0, 3.0, 3.0),
                glm::vec3(0.0, 0.0, 0.0),
                glm::vec3(0.0, 1.0, 0.0));
    mModel = glm::mat4(1.0);
    mMvp = mProjection * mView * mModel;

    mShaderMvp = mShader->uniformId("MVP");
}

Scene::~Scene()
{
    delete mShader;


    SDL_GL_DeleteContext(mContext);
    SDL_DestroyWindow(mWindow);
}

int Scene::start() {
    while (mRunning) {
        flushEvents();
        draw();
        SDL_GL_SwapWindow(mWindow);
    }
    return 0;
}

void Scene::draw() {
    mShader->bind();

    glUniformMatrix4fv(mShaderMvp, 1, GL_FALSE, &mMvp[0][0]);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glDisableVertexAttribArray(0);

    mShader->unbind();
}

void Scene::flushEvents() {
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE)
                mRunning = false;
            break;
        case SDL_QUIT:
            mRunning = false;
            break;
        }
    }
}
