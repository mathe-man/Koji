#pragma once

struct Scene;


class Application {
public:
    static bool Run(Scene* scene);

    static Scene* scene;
};
