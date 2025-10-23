#include "imgui.h"
#include  "raylib.h"
#include "raymath.h"
namespace Koji::Components
{
    class kComponent {
    public:
        virtual ~kComponent() = default;
        [[nodiscard]] virtual const char* GetName() const = 0;
        virtual void Inspect() = 0;
    };

    class kTransform : public kComponent {
        [[nodiscard]] const char* GetName() const override { return "kTransform"; }
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
        
        Vector3 position  {0.0f, 0.0f, 0.0f};
        Vector4 rotation  {1.0f, 0.0f, 0.0f, 0.0f};
        Vector3 scale     {1.f, 1.f, 1.f};
    };
    
    class kVelocity : kComponent {
        [[nodiscard]] const char* GetName() const override { return "kVelocity"; }
        void Inspect() override {
            ImGui::DragFloat3("Linear velocity", &LinearVelocity.x, 0.1f);
        }
        
        Vector3 LinearVelocity  = {};
        // TODO implement angular velocity
    };
}