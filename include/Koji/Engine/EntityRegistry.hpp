#pragma once
#include <typeindex>
#include "entt/entt.hpp"

namespace Koji::Engine
{
    class kComponent;
    
    
    // TODO replace entt registry by this class in the engine
    class EntityRegistry : public entt::registry {
    public:
        using entt::registry::registry;

        template<typename T, typename... Args>
        T* AddComponent(entt::entity entity, Args&&... args) {
            if constexpr (std::is_base_of_v<kComponent, T>) {
                return &entt::registry::emplace<T>(entity, std::forward<Args>(args)...);
            }
            return nullptr;
        }

    private:
        // Make EnTT original method unavailable
        using entt::registry::emplace;
        using entt::registry::emplace_or_replace;
    };
    
} // namespace Koji::Engine
