#pragma once

namespace Koji::Engine
{
    struct Scene;

    
    class Application {
    public:
        static bool Run(Scene* scene);

        static Scene* scene;
    };
} // namespace Koji::Engine