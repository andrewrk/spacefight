#ifndef SCENE_H
#define SCENE_H

class Scene
{
public:
    Scene();
    void mainLoop();
    int start();
    void flushEvents();
private:
    bool m_running = true;
};

#endif // SCENE_H
