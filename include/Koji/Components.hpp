#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <functional>





namespace Koji::Engine
{
    class kComponent {
    public:
        virtual ~kComponent() = default;
        [[nodiscard]] virtual const char* GetName() const = 0;

        // dictionnaire global pour toutes les factories
        static std::unordered_map<std::string, std::function<std::unique_ptr<kComponent>()>>& RegisteredComponents() {
            static std::unordered_map<std::string, std::function<std::unique_ptr<kComponent>()>> map;
            return map;
        }

        // helper pour créer un composant par nom
        static std::unique_ptr<kComponent> Create(const std::string& name) {
            auto it = RegisteredComponents().find(name);
            if (it != RegisteredComponents().end())
                return it->second(); // appelle la lambda
            return nullptr;
        }

        // helper pour lister tous les composants
        static std::vector<std::string> GetRegisteredNames() {
            std::vector<std::string> names;
            for (auto& [name, _] : RegisteredComponents())
                names.push_back(name);
            return names;
        }
    };

#define REGISTER_COMPONENT(T) \
    static_assert(std::is_base_of_v<kComponent, T>, #T " must derive from kComponent"); \
    namespace { \
        struct T##_registrar { \
            T##_registrar() { \
                kComponent::RegisteredComponents()[T().GetName()] = []() -> std::unique_ptr<kComponent> { \
                    return std::make_unique<T>(); \
                }; \
            } \
        }; \
        static T##_registrar global_##T##_registrar; \
    }
    
namespace Components
{
    class kMetaData : public kComponent {
    public:
        [[nodiscard]] const char* GetName() const override { return "kMetaData"; }
        
        
        char name[128] =  "Entity";
        entt::entity parent = entt::null;
        std::vector<entt::entity> children = std::vector<entt::entity>();

        
    };
    REGISTER_COMPONENT(kMetaData)
    
    class kTransform : public kComponent {
    public:
        [[nodiscard]] const char* GetName() const override { return "kTransform"; }
        
        Vector3 position  {0.0f, 0.0f, 0.0f};
        Vector4 rotation  {1.0f, 0.0f, 0.0f, 0.0f};
        Vector3 scale     {1.f, 1.f, 1.f};

    };
    REGISTER_COMPONENT(kTransform)
    
    class kVelocity : public kComponent {
    public:
        [[nodiscard]] const char* GetName() const override { return "kVelocity"; }
        
        
        Vector3 LinearVelocity  = {};
        // TODO implement angular velocity
    };
    REGISTER_COMPONENT(kVelocity)

    class kSphere : public kComponent {
    public:
        [[nodiscard]] const char* GetName() const override { return "kSphere"; }
        

        float radius = 3.0f;
        int rings = 5;
        int slices = 5;
        Color color = VIOLET;
    };

    REGISTER_COMPONENT(kSphere)
     

} // namespace Koji::Engine::Components
} // namespace Koji::Engine