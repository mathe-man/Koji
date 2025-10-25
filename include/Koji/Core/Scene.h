#pragma once
#include <cstdint>
#include "entt/entity/registry.hpp"

// Forward de
namespace Koji::Systems { class System; }

namespace Koji::Core {

    struct Scene
    {
        const char* name;
        uint16_t window_width;
        uint16_t window_height;
        entt::registry registry;
        std::vector<Systems::System*> systems;
    };

    class Application {
    public:
        static bool Run(Scene* scene);

        static Scene* scene;
    };

} // Koji