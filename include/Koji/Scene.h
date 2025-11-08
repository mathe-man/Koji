#pragma once
#include <string>
#include <vector>

#include "ECS/System.h"
#include "ECS/World.h"

namespace Koji
{
    class Scene
    {
    private:
        std::unique_ptr<ECS::World> world;
        std::vector<std::unique_ptr<ECS::System>> systems;
        bool isRunning = false;
        std::string name;
        
    public:
        Scene(const std::string& sceneName)
            : name(sceneName), world(std::make_unique<ECS::World>()) {
        }
        ~Scene() = default;

        ECS::World* GetWorld() { return world.get(); }
        
        // Systems management
        template<typename T, typename... Args>
        T* AddSystem(Args&&... args);
        template<typename T>
        T* GetSystem();
        template<typename T>
        bool RemoveSystem();
        
        bool Load();
        bool Unload();
        bool Update();
        
        [[nodiscard]] const std::string& GetName() const { return name; }
        [[nodiscard]] bool IsRunning() const { return isRunning; }
    };
}
