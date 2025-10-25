#pragma once
#include <Koji/Engine.h>

namespace Koji::Editor
{
    
    class Editor {
    public:
        static bool Run(Engine::Scene* scene);

        static Engine::Scene* scene;
    };
}