#pragma once
#include <entt.hpp>
#include <Koji/Render/Renderer.h>
#include <entt.hpp>

namespace Koji {
    class RenderingSystem {
    public:
        RenderingSystem(Renderer* renderer);
        void Render(entt::registry& registry) const;

    private:
        Renderer* m_renderer;
    };
} // Koji