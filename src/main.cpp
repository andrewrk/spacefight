#include "scene.h"

int main()
{
    srand(time(NULL));
    Scene *scene = new Scene();
    return scene->start();
}

