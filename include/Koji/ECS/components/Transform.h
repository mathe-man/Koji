#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Koji {

struct Transform {
    glm::vec3 position{0.0f};
    glm::quat rotation;
    glm::vec3 scale{1.f, 1.f, 1.f};
};

} // Koji