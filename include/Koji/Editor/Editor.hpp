#pragma once

namespace Koji::Engine
{ struct Scene; }

namespace Koji::Editor
{
    
    class Editor {
    public:
        static bool Run(Engine::Scene* scene);

        static Engine::Scene* scene;
    };
}