#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <functional>

#include "imgui.h"
#include "raymath.h"



namespace Koji::Engine
{
    class kComponent {
    public:
        virtual ~kComponent() = default;
        [[nodiscard]] virtual const char* GetName() const = 0;
        virtual void Inspect() = 0;

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
                kComponent::RegisteredComponents()[T().GetName()] = []() { return std::make_unique<T>(); }; \
            } \
        }; \
        static T##_registrar global_##T##_registrar; \
    }

    
namespace Components
{
    class kMetaData : kComponent {
    public:
        [[nodiscard]] const char* GetName() const override { return "kMetaData"; }

        char* name;
        entt::entity parent = entt::null;
        std::vector<entt::entity> children = std::vector<entt::entity>();

        void Inspect() override {
            ImGui::InputText("Name", name, 128, ImGuiInputTextFlags_AutoSelectAll);
            ImGui::Text("Number of children: %s", children.size());
        };
    };
    
    class kTransform : public kComponent {
    public:
        [[nodiscard]] const char* GetName() const override { return "kTransform"; }
        
        Vector3 position  {0.0f, 0.0f, 0.0f};
        Vector4 rotation  {1.0f, 0.0f, 0.0f, 0.0f};
        Vector3 scale     {1.f, 1.f, 1.f};
        
        void Inspect() override {
            // --- Position ---
            ImGui::DragFloat3("position", &position.x, 0.1);

            // --- Rotation ---
            Quaternion q = {rotation.x, rotation.y, rotation.z, rotation.w};

            // Convert quaternion to Euler angles (radians)
            auto [x, y, z] = QuaternionToEuler(q);

            // Convert radians to degrees
            Vector3 eulerDeg = { x * 180.0f / PI,
                                 y * 180.0f / PI,
                                 z * 180.0f / PI };

            if (ImGui::DragFloat3("Rotation", &eulerDeg.x, 0.5f, -180.0f, 180.0f)) {
                // Convert back to radians
                Vector3 newEulerRad = { eulerDeg.x * PI / 180.0f,
                                        eulerDeg.y * PI / 180.0f,
                                        eulerDeg.z * PI / 180.0f };

                // Convert Euler to quaternion
                Quaternion newQuat = QuaternionFromEuler(newEulerRad.z, newEulerRad.y, newEulerRad.x);

                // Normalize quaternion
                newQuat = QuaternionNormalize(newQuat);

                // Store back
                rotation.x = newQuat.x;
                rotation.y = newQuat.y;
                rotation.z = newQuat.z;
                rotation.w = newQuat.w;
            }

            // --- Scale ---
            ImGui::DragFloat3("Scale", &scale.x, 0.1);
        }
    };
    REGISTER_COMPONENT(kTransform)
    
    class kVelocity : public kComponent {
    public:
        [[nodiscard]] const char* GetName() const override { return "kVelocity"; }
        void Inspect() override {
            ImGui::DragFloat3("Linear velocity", &LinearVelocity.x, 0.1f);
        }
        
        Vector3 LinearVelocity  = {};
        // TODO implement angular velocity
    };
    REGISTER_COMPONENT(kVelocity)

} // namespace Koji::Engine::Components
} // namespace Koji::Engine