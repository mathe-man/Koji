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
        T* AddSystem(Args&&... args){
            static_assert(std::is_base_of_v<ECS::System, T>, "T must derive from System");
    
            auto system = std::make_unique<T>(std::forward<Args>(args)...);
            T* ptr = system.get();
            systems.push_back(std::move(system));
            return ptr;
        }
        template<typename T>
        T* GetSystem(){
            for (auto& system : systems) {
                if (auto* casted = dynamic_cast<T*>(system.get())) {
                    return casted;
                }
            }
            return nullptr;
        }
        template<typename T>
        bool RemoveSystem(){
            static_assert(std::is_base_of_v<ECS::System, T>, "T must derive from System");
    
            auto it = std::find_if(systems.begin(), systems.end(),
                [](const std::unique_ptr<ECS::System>& system) {
                    return dynamic_cast<T*>(system.get()) != nullptr;
                });
    
            if (it != systems.end()) {
                // Call Close() to clean
                (*it)->Close();
        
                systems.erase(it);
        
                return true;
            }
            return false;
        }
        
        bool Load();
        bool Unload();
        bool Update();
        
        [[nodiscard]] const std::string& GetName() const { return name; }
        [[nodiscard]] bool IsRunning() const { return isRunning; }
    };
}
