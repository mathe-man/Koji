#include <stdexcept>
#include <Koji/Scene.h>

using namespace Koji;


template<typename T, typename... Args>
T* Scene::AddSystem(Args&&... args) {
    static_assert(std::is_base_of_v<ECS::System, T>, "T must derive from System");
    
    auto system = std::make_unique<T>(std::forward<Args>(args)...);
    T* ptr = system.get();
    systems.push_back(std::move(system));
    return ptr;
}

template<typename T>
T* Scene::GetSystem() {
    for (auto& system : systems) {
        if (auto* casted = dynamic_cast<T*>(system.get())) {
            return casted;
        }
    }
    return nullptr;
}

template<typename T>
bool Scene::RemoveSystem() {
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

bool Scene::Load() {
    isRunning = true;
    
    for (auto& system : systems)
        if (!system->Init()) 
            return false;
    return true;
}

bool Scene::Update() {
    if (!isRunning) return false;
    
    for (auto& system : systems)
        if (!system->Update())
            return false;
    return true;
}

bool Scene::Unload() {
    isRunning = false;
    
    for (auto& system : systems)
        if (!system->Close())
            return false;
    return true;
}