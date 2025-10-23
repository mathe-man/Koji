#pragma once
#include <iostream>
#include <entt/entt.hpp>

#include "Components/Components.hpp"
#include "Systems/Systems.hpp"

using namespace Koji::Components;
using namespace Koji::Systems;

// TODO replace entt registry by this class in the engine
class EntityManager : public entt::registry {
public:
    using entt::registry::registry;

    template<typename T, typename... Args>
    T* AddComponent(entt::entity entity, Args&&... args) {
        if constexpr (std::is_base_of_v<kComponent, T>) {
            return &entt::registry::emplace<T>(entity, std::forward<Args>(args)...);
        } else {
            // std::cerr
            std::cout
            << "[ECS] error: tried to add invalid class of component: '"
            << typeid(T).name() << "' is not derived from class kComponent\n";
            return nullptr;
        }
    }

private:
    // Make EnTT original method unavailable
    using entt::registry::emplace;
    using entt::registry::emplace_or_replace;
    
};