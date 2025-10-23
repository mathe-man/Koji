#pragma once
#include <cstdint>

#include "entt/entity/registry.hpp"
#include "Koji/ECS/Systems/Systems.hpp"

namespace Koji::Core {

    struct ApplicationData
    {
        const char* name;
        uint16_t window_width;
        uint16_t window_height;
        entt::registry registry;
        std::vector<Systems::System*> systems;
    };

    class Application {
    public:
        static bool Init(ApplicationData d);
        static bool Run(bool exit_at_end = true);
        static void Exit();

        static ApplicationData data;

    private:
        static bool as_initiated;
    };

} // Koji